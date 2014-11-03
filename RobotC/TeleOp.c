#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     Michelangelo_FR, tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     Donatello_FL,  tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     Raphael_BR,    tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     Leonardo_BL,   tmotorTetrix, PIDControl, encoder)
#pragma config(Servo,  srvo_S1_C3_1,    Servo_Tube,           tServoStandard)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "Headers\Motor.h"
#include "Headers\Servo.h"
#include "Headers\JoyMecanumDrive.h"

//Stores desired motor values
DesiredMotorVals desiredMotorVals;

void initialize() {
	//servoInit();
	motorInit();
	//Initialize to zeroes
	memset(&desiredMotorVals, 0, sizeof(desiredMotorVals));
	//servoSetAngle(Servo_Tube, 75);
}

task main() {
	initialize();
	waitForStart();
	while (true) {
		joyUpdateJoystickSettings();
		joymecdriveSetDesiredPower(&desiredMotorVals, joyGetJoystickPointer());
		//driveSetMecMotorPolarDegrees(&desiredMotorVals, 90, 1.0, 0);
		motorSetActualPowerToDesired(&desiredMotorVals);
	}
}
