#pragma config(Hubs,  S2, HTServo,  none,     none,     none)
#pragma config(Hubs,  S3, HTMotor,  HTMotor,  HTMotor,  HTMotor)
#pragma config(Sensor, S2,     ,               sensorI2CMuxController)
#pragma config(Sensor, S3,     ,               sensorI2CMuxController)
#pragma config(Sensor, S4,     HTSMUX,         sensorI2CCustom)
#pragma config(Motor,  mtr_S3_C1_1,     Michelangelo_FR, tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S3_C1_2,     Donatello_FL,  tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S3_C2_1,     Raphael_BR,    tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S3_C2_2,     Leonardo_BL,   tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S3_C3_1,     HarvesterMove, tmotorTetrix, openLoop, encoder)
#pragma config(Motor,  mtr_S3_C3_2,     HarvesterDuo,  tmotorTetrix, openLoop, reversed)
#pragma config(Motor,  mtr_S3_C4_1,     Lift,					 tmotorTetrix, openLoop, reversed, encoder)
#pragma config(Motor,  mtr_S3_C4_2,     Harvester,     tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S2_C1_1,    servo1,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_2,    servo2,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_3,    servo3,        tServoNone)
#pragma config(Servo,  srvo_S2_C1_4,    TubeGrabber, 	 tServoStandard)
#pragma config(Servo,  srvo_S2_C1_5,    Bucket,			   tServoStandard)
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

#include "Headers\Global.h"
#include "Headers\Motor.h"
#include "Headers\Servo.h"
#include "Headers\JoyMecanumDrive.h"
#include "Headers\JoyAuxiliary.h"

//Stores desired motor values
DesiredMotorVals desiredMotorVals;
//Stores desired encoder values
DesiredEncVals desiredEncVals;

void initialize() {
	clearDebugStream();
	writeDebugStream("This is TeleOp.c\n");
	//Initialize to zeroes
	memset(&desiredMotorVals, 0, sizeof(desiredMotorVals));
	memset(&desiredEncVals, 0, sizeof(desiredEncVals));
	motorInit(&desiredEncVals);
	servoInit();
}

void callAuxiliaryMotors(){
	joyLift(&desiredMotorVals, &desiredEncVals, joyGetJoystickPointer());
	joyHarvester(&desiredMotorVals, joyGetJoystickPointer());
	joyGrabber(&desiredMotorVals, joyGetJoystickPointer());
	joyBucketDrop(&desiredMotorVals, joyGetJoystickPointer());
}

task main() {
	initialize();
	joyWaitForStart();

	while (true) {
		long loopStartTimeMs = nPgmTime;

		joyUpdateJoystickSettings();
		joymecdriveSetDesiredPower(&desiredMotorVals, joyGetJoystickPointer());
		//joymecdriveDebug(&desiredMotorVals, &desiredEncVals, joyGetJoystickPointer());
		callAuxiliaryMotors();
		motorLimitDesiredPowerToEncoder(&desiredMotorVals, &desiredEncVals);
		motorSetActualPowerToDesired(&desiredMotorVals);
		motorUpdateState();
		//joyAuxDebug(&desiredMotorVals, joyGetJoystickPointer());


		//IR code
		/*int dcS1, dcS2, dcS3, dcS4, dcS5 = 0;
		int acS1, acS2, acS3, acS4, acS5 = 0;
		int dirEnh, strEnh = 0; //direction, strength enhanced

		int dirDC = HTIRS2readDCDir(HTIRS2);
		int dirAC = HTIRS2readACDir(HTIRS2);
		HTIRS2readAllDCStrength(HTIRS2, dcS1, dcS2, dcS3, dcS4, dcS5);
		HTIRS2readAllACStrength(HTIRS2, acS1, acS2, acS3, acS4, acS5);
		HTIRS2readEnhanced(HTIRS2, dirEnh, strEnh);
		/*writeDebugStream("IR reads: ");
		writeDebugStream("DC: %d AC: %d\n", dirDC, dirAC);
		writeDebugStream("D: %d %d %d %d %d\n", dcS1, dcS2, dcS3, dcS4, dcS5);
		writeDebugStream("A: %d %d %d %d %d\n", acS1, acS2, acS3, acS4, acS5);
		writeDebugStream("Enh Dir: %d Str %d\n", dirEnh, strEnh);


		//ULTRA code
		int ultraDistance = 0;
		ultraDistance = USreadDist(LEGOUS);
		writeDebugStream("Dist:  %3d cm\n", ultraDistance);

		writeDebugStream("DMV: %d %d %d %d\n", desiredMotorVals.power[MecMotor_FL], desiredMotorVals.power[MecMotor_BL], desiredMotorVals.power[MecMotor_FR], desiredMotorVals.power[MecMotor_BR]);*/
		//writeDebugStream("Full teleop loop took: %d ms\n", nPgmTime - loopStartTimeMs);
	}
}
