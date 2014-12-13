#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Hubs,  S3, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Sensor, S3,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     Chute_Right,   tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Slides_Right,  tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     Michelangelo_FR, tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     Raphael_BR,    tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S3_C1_1,     Donatello_FL,  tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S3_C1_2,     Leonardo_BL,   tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S3_C2_1,     Chute_Left,    tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S3_C2_2,     Slides_Left,   tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S3_C3_1,    KickStand,            tServoStandard)
#pragma config(Servo,  srvo_S3_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S3_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S3_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S3_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S3_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "Headers\Motor.h"
#include "Headers\Servo.h"
#include "Headers\JoyMecanumDrive.h"
#include "Headers\Global.h"
#include "Headers\JoyAuxiliary.h"

//Stores desired motor values
DesiredMotorVals desiredMotorVals;

void initialize() {
	servoInit();
	motorInit();
	//Initialize to zeroes
	clearDebugStream();
	memset(&desiredMotorVals, 0, sizeof(desiredMotorVals));
}

void callAuxiliaryMotors(){
	joyChuteMotors(&desiredMotorVals, joyGetJoystickPointer());
	joySlideMotors(&desiredMotorVals, joyGetJoystickPointer());
}

task main() {
	initialize();
	waitForStart();
	while (true) {
		joyUpdateJoystickSettings();
		joymecdriveSetDesiredPower(&desiredMotorVals, joyGetJoystickPointer());
		//driveSetMecMotorPolarDegrees(&desiredMotorVals, 90, 1.0, 1.0);
		motorSetActualPowerToDesired(&desiredMotorVals);
		callAuxiliaryMotors();
		writeDebugStreamLine("------");
	}
}
