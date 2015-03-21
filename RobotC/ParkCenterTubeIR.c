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
#pragma config(Motor,  mtr_S3_C3_2,     LiftEncoder,        tmotorTetrix, openLoop, encoder, reversed)
#pragma config(Motor,  mtr_S3_C4_1,     Lift,					 tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S3_C4_2,     Harvester,     tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S2_C1_1,    servo1,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_2,    servo2,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_3,    servo3,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_4,    TubeGrabber, 			 tServoStandard)
#pragma config(Servo,  srvo_S2_C1_5,    Bucket,   tServoStandard)
#pragma config(Servo,  srvo_S2_C1_6,    servo4,        tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// The sensor is connected to the first port
// of the SMUX which is connected to the NXT port S1.
// To access that sensor, we must use msensor_S1_1.  If the sensor
// were connected to 3rd port of the SMUX connected to the NXT port S4,
// we would use msensor_S4_3

#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-irseeker-v2.h"
#include "drivers/hitechnic-gyro.h"
#include "drivers/lego-ultrasound.h"

// Give the sensor a nice easy to use name
const tMUXSensor GYRO = msensor_S4_1;
const tMUXSensor HTIRS2 = msensor_S4_4;
const tMUXSensor LEGOUS = msensor_S4_3;

#include "Headers\Joystick.h"
#include "Headers\Motor.h"
#include "Headers\Servo.h"
#include "Headers\Global.h"
#include "Headers\Drive.h"
#include "Auton\Auton.h"

//ALL states for center Tube Autonomous
typedef enum CenterTubeStates {
	STATE_START, //
	STATE_DRIVETOWARDCENTERPIECE, //
	STATE_DETECTIR,
	STATE_ORBITTOCENTERGOAL,//
	STATE_DROPBUCKET, //
	STATE_DRIVERIGHT, //
	STATE_KICKSTAND, //
	STATE_WAITFORLIFT,
	STATE_END,
	STATE_SHORTBUCKET,
	STATE_CLOSEBUCKET,
} CenterTubeStates;

static CenterTubeStates currentState = STATE_START;

//States for center position
typedef enum CenterPos {
	HORIZ_TO_PARK,
	ANGLE_TO_PARK,
	VERT_TO_PARK,
} CenterPos;

//IR Sensor
int dirEnh, strEnh = 0; //direction, strength enhanced

task main()
{
	//SLIDE SIDE IS FACING DOWN THE RAMP
	CenterPos centerPos;
	bool end = false;
	bool init = true;
	long bucketStartTimeMs = 0;

	initialize();
	writeDebugStream("This is CenterTubeIR\n");
	waitForStart();

	while(!end){
		long loopStartTimeMs = nPgmTime;
		if (rest) {
			writeDebugStream("Resting!\n");
			motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
			motorSetActualPowerToDesired(&desiredMotorVals);
			motorUpdateState();
			if (nPgmTime - restStartTimeMs > 2000) {
				rest = false;
				restStartTimeMs = 0;
			}
		} else {
			switch(currentState){
				case STATE_START:
					currentState = STATE_DRIVETOWARDCENTERPIECE;
					break;
				case STATE_DRIVETOWARDCENTERPIECE:
					writeDebugStream("State: Drive Toward Centerpiece\n");
					if (init) {
						driveSetEncoderN(&desiredEncVals, 1.0 * ENC_PER_REV);
						init = false;
					}

					driveSetMecMotorN(&desiredMotorVals, 0.30);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);
					motorUpdateState();

					if (driveMecHasHitEncoderTarget(&desiredEncVals)) {
						restMecMotors();
						currentState = STATE_DETECTIR;
						init = true;
					}
					break;
				case STATE_DETECTIR:
					writeDebugStream("State: Detect IR\n");
					//Read average of 10 values
					long sum = 0;
					for (int i=0; i<10; i++) {
						HTIRS2readEnhanced(HTIRS2, dirEnh, strEnh);
						sum += strEnh;
					}
					long avgStrEnh = sum / 10;

					writeDebugStream("IR Str Enh: %d\n", avgStrEnh);

					if (avgStrEnh > 90) {
						writeDebugStream("Detected vertical to park\n");
						centerPos = VERT_TO_PARK;
					} else if (avgStrEnh > 40) {
						writeDebugStream("Detected angle to park\n");
						centerPos = ANGLE_TO_PARK;
					} else {
						writeDebugStream("Detected horizontal to park\n");
						centerPos = HORIZ_TO_PARK;
					}

					if (centerPos == VERT_TO_PARK) {
						currentState = STATE_WAITFORLIFT;
						init = true;
					} else {
						currentState = STATE_ORBITTOCENTERGOAL;
						init = true;
					}
					break;
			case STATE_ORBITTOCENTERGOAL:
					writeDebugStream("State: Orbit to center goal\n");
					if (init) {
						driveSetEncoderOrbitN(&desiredEncVals, 0.3, 0.3 * ENC_PER_REV);
						init = false;
					}

					driveSetMecMotorOrbitN(&desiredMotorVals, 0.3);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);
					motorUpdateState();

					if (driveMecHasHitEncoderTarget(&desiredEncVals)) {
						restMecMotors();
						currentState = STATE_WAITFORLIFT;
						init = true;
					}
					break;
				case STATE_WAITFORLIFT:
					writeDebugStream("State: WAIT FOR LIFT\n");
					desiredMotorVals.power[Lift] = LIFT_UP;
					motorSetEncoder(&desiredEncVals, Lift, LIFT_MAX, ENCFLAGS_CAPMODE_ON ^ ENCFLAGS_CAPMAX_ON);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);
					motorUpdateState();

					if (motorHasHitEncoderTarget(&desiredEncVals, Lift)) {
						restMecMotors();
						currentState = STATE_DROPBUCKET;
						init = true;
					}
					break;
				case STATE_DROPBUCKET:
					writeDebugStream("State: Drop Bucket\n");
					if (init) {
						bucketStartTimeMs = nPgmTime;
						servoSetNonCont(Bucket, servoDefinitions[Bucket].minValue);
						init = false;
					} else {
						if ((nPgmTime - bucketStartTimeMs) > 3000) { //balls need time to drop
							restMecMotors();
							currentState = STATE_CLOSEBUCKET;
							init = true;
						}
					}
					break;
				case STATE_CLOSEBUCKET:
					writeDebugStream("State: Close bucket\n");
					if (init) {
						bucketStartTimeMs = nPgmTime;
						servoSetNonCont(Bucket, servoDefinitions[Bucket].maxValue);
						init = false;
					} else {
						if ((nPgmTime - bucketStartTimeMs) > 1000) {
							restMecMotors();
							currentState = STATE_DRIVERIGHT;
							init = true;
						}
					}
					break;
				case STATE_DRIVERIGHT:
					writeDebugStream("State: Drive Right Toward Kickstand\n");
					if (init) {
						driveSetEncoderE(&desiredEncVals, 2000);
						init = false;
					}

					driveSetMecMotorE(&desiredMotorVals, 0.30);
					desiredMotorVals.power[Lift] = LIFT_DOWN;
					motorSetEncoder(&desiredEncVals, Lift, LIFT_MIN, ENCFLAGS_CAPMODE_ON);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);
					motorUpdateState();

					if (driveMecHasHitEncoderTarget(&desiredEncVals)) {
						restMecMotors();
						currentState = STATE_KICKSTAND;
						init = true;
					}
					break;
				case STATE_KICKSTAND:
					writeDebugStream("State: Drive Kickstand\n");
					if (init) {
						driveSetEncoderN(&desiredEncVals, 2000);
						init = false;
					}

					driveSetMecMotorN(&desiredMotorVals, 0.30);
					desiredMotorVals.power[Lift] = LIFT_DOWN;
					motorSetEncoder(&desiredEncVals, Lift, LIFT_MIN, ENCFLAGS_CAPMODE_ON);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);
					motorUpdateState();

					if (driveMecHasHitEncoderTarget(&desiredEncVals)) {
						restMecMotors();
						currentState = STATE_END;
						init = true;
					}
					break;
				case STATE_END:
					desiredMotorVals.power[Lift] = LIFT_DOWN;
					motorSetEncoder(&desiredEncVals, Lift, LIFT_MIN, ENCFLAGS_CAPMODE_ON);

					motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
					motorSetActualPowerToDesired(&desiredMotorVals);
					motorUpdateState();

					if(motorHasHitEncoderTarget(&desiredEncVals, Lift)) {
						restMecMotors();
						end = true;
					}
					break;
			} //switch
		} //if
	//writeDebugStream("DMV: %d %d %d %d\n", desiredMotorVals.power[MecMotor_FL], desiredMotorVals.power[MecMotor_BL], desiredMotorVals.power[MecMotor_FR], desiredMotorVals.power[MecMotor_BR]);
	//writeDebugStream("VIRTENC: %d %d %d %d\n", motorGetEncoder((tMotor) MecMotor_FL), motorGetEncoder((tMotor) MecMotor_BL), motorGetEncoder((tMotor) MecMotor_FR), motorGetEncoder((tMotor) MecMotor_BR);
	//writeDebugStream("REALENC: %d %d %d %d\n", nMotorEncoder[Donatello_FL], nMotorEncoder[Leonardo_BL], nMotorEncoder[Michelangelo_FR], nMotorEncoder[Raphael_BR]);
	//writeDebugStream("Full auton loop took: %d ms\n", nPgmTime - loopStartTimeMs);
	} //while

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
