#pragma config(Hubs,  S2, HTServo,  none,     none,     none)
#pragma config(Hubs,  S3, HTMotor,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     ,               sensorI2CMuxController)
#pragma config(Sensor, S3,     ,               sensorI2CMuxController)
#pragma config(Sensor, S4,     HTSMUX,         sensorI2CCustom)
#pragma config(Motor,  mtr_S3_C1_1,     Michelangelo_FR, tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S3_C1_2,     Donatello_FL,  tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S3_C2_1,     Raphael_BR,    tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S3_C2_2,     Leonardo_BL,   tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S3_C3_1,     HarvesterMove, tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S3_C3_2,     motor0,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S3_C4_1,     Lift,					 tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S3_C4_2,     Harvester,     tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S2_C1_1,    servo1,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_2,    servo2,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_3,    servo3,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_4,    servo4, 			 tServoNone)
#pragma config(Servo,  srvo_S2_C1_5,    TubeGrabber,   tServoStandard)
#pragma config(Servo,  srvo_S2_C1_6,    Bucket,        tServoStandard)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// The sensor is connected to the first port
// of the SMUX which is connected to the NXT port S1.
// To access that sensor, we must use msensor_S1_1.  If the sensor
// were connected to 3rd port of the SMUX connected to the NXT port S4,
// we would use msensor_S4_3

#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-gyro.h"

// Give the sensor a nice easy to use name
const tMUXSensor GYRO = msensor_S4_1;

#include "..\Headers\Joystick.h"
#include "..\Headers\Motor.h"
#include "..\Headers\Servo.h"
#include "..\Headers\Global.h"
#include "..\Headers\Drive.h"

//Stores desired motor values
DesiredMotorVals desiredMotorVals;
//Stores desired encoder values
DesiredEncVals desiredEncVals;

//ALl states for Grab Medium Tube Autonomous
typedef enum GrabMedTubeStates {
	STATE_START,
	STATE_DRIVETOWARDCENTERPIECE,
	STATE_DRIVETOWARDTUBE,
	STATE_ROTATETOWARDWALL,
	STATE_DRIVETOWARDPZONE,
	STATE_ALIGNANDLOCKTUBE,
	STATE_OFFRAMP,
	STATE_LIFT,
	STATE_DROPBUCKET,
	STATE_GRABTUBE,
	STATE_LOCKTUBEIN,
	STATE_DROP,
	STATE_END,
} GrabMedTubeStates;

static GrabMedTubeStates currentState = STATE_START;

void initialize(){
	servoInit();
	motorInit(&desiredEncVals);
	motorResetAllEncoders(&desiredEncVals);
	memset(&desiredMotorVals, 0, sizeof(desiredMotorVals));
	memset(&desiredEncVals, 0, sizeof(desiredEncVals));
}


TTimers AutonTimer = T1;

#define ENC_PER_REV 1440

long restStartTimeMs = 0;
long liftStartTimeMs = 0;
bool rest = false;

void restMecMotors() {
	driveResetMecEncoder(&desiredEncVals);
	driveZeroMecMotor(&desiredMotorVals);
	rest = true;
	restStartTimeMs = nPgmTime;
}


task main()
{
	//SLIDE SIDE IS FACING DOWN THE RAMP

	initialize();
	waitForStart();
	bool end = false;
	bool rest  = false;
	while(!end){
		if (rest) {
			if (nPgmTime - restStartTimeMs > 1000) {
				rest = false;
				restStartTimeMs = 0;
			} else {
				motorZeroAllMotors(&desiredMotorVals);
			}
		} else {
			switch(currentState){
				case STATE_START:
					currentState = STATE_OFFRAMP;
					break;
				case STATE_OFFRAMP:
					writeDebugStream("State: Offramp\n");
					driveSetEncoderN(&desiredEncVals, 8.029685 * ENC_PER_REV);
					driveSetMecMotorN(&desiredMotorVals, 0.5);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);

					if(driveMecHasHitEncoderTarget(&desiredEncVals)){
						restMecMotors();
						currentState = STATE_END;//STATE_DRIVETOWARDCENTERPIECE;
					}
					break;
			/*case STATE_DRIVETOWARDCENTERPIECE:
					writeDebugStream("State: Drive Toward Centerpiece\n");
					driveSetEncoderE(&desiredEncVals, .267562016 * ENC_PER_REV);
					driveSetMecMotorE(&desiredMotorVals, 0.5);

					liftStartTimeMs = nPgmTime;
					desiredMotorVals.power[Lift] = 100;

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);

					if ((nPgmTime - liftStartTimeMs) > 3500) {
							desiredMotorVals.power[Lift] = 0;
					}

					if(driveMecHasHitEncoderTarget(&desiredEncVals)) {
						restMecMotors();
						currentState = STATE_DRIVETOWARDTUBE;
					}
					break;
				case STATE_DRIVETOWARDTUBE:
					writeDebugStream("State: Drive Toward Tube\n");
					driveSetEncoderN(&desiredEncVals, .2 * ENC_PER_REV);
					driveSetMecMotorN(&desiredMotorVals, 0.3);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);

					if ((nPgmTime - liftStartTimeMs) > 3500) {
							desiredMotorVals.power[Lift] = 0;
					}

					if (driveMecHasHitEncoderTarget(&desiredEncVals) &&
							motorHasHitEncoderTarget(&desiredEncVals, Lift)) {
						restMecMotors();
						currentState = STATE_GRABTUBE;
					}
					break;
				case STATE_GRABTUBE:
					writeDebugStream("State: Grab Tube\n");
					servoSetNonCont(TubeGrabber, servoDefinitions[TubeGrabber].maxValue);

					if ((nPgmTime - liftStartTimeMs) > 3500) {
							desiredMotorVals.power[Lift] = 0;
					}

					restMecMotors(); //wait for servo to go down
					if (desiredMotorVals.power[Lift] == 0) {
						currentState = STATE_DROP;
					}
					break;
				case STATE_DROP:
					writeDebugStream("State: Drop\n");
				//time1[T3] = 0;
					servoSetNonCont(Bucket_Drop, servoDefinitions[Bucket_Drop].maxValue);
					currentState = STATE_ROTATETOWARDCENTERPIECE;
					break;
				case STATE_ROTATETOWARDCENTERPIECE:
					writeDebugStream("State: Rotate Toward Centerpiece\n");
					driveSetEncoderRotate(&desiredEncVals, 5);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);

					if(motorAllHitEncoderTarget(&desiredEncVals)) {
						resetAndRest();
						currentState = STATE_DRIVETOWARDPZONE;
					}
					break;
				case STATE_DRIVETOWARDPZONE:
					writeDebugStream("State: Drive Toward Zone\n");
					driveSetEncoderS(&desiredEncVals, 6 * ENC_PER_REV);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);

					if(motorAllHitEncoderTarget(&desiredEncVals)){
						currentState = STATE_END;
					}
					break;
	*/			case STATE_END:
					end = true;
					break;
			}
		}
	}

/*	driveSetEncoderN(&desiredEncVals, ######);
	driveSetEncoderW(&desiredEncVals, ######);
	driveSetEncoderN(&desiredEncVals, ######);
	servoSetNonCont(Wing_Base, servoDefinitions[Wing_Base].minValue);
	driveSetEncoderS(&desiredEncVals, ######);
	desiredMotorVals->power[Wing_Middle] = -100;
	time[T1] = 0;
	while (time[T1] < 200){
		motorSetActualPowerToDesired(&desiredMotorVals);
	}
	desiredMotorVals->power[Wing_Middle] = 0;
	motorSetActualPowerToDesired(&desiredMotorVals);
	driveSetEncoderW(&desiredEncVals, ######);
	driveSetEncoderS(&desiredEncVals, ######);
	driveSetEncoderW(&desiredEncVals, ######); */
}
