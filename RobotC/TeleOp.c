#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C1_1,     Michelangelo_FR,  tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C1_2,     Donatello_FL,  tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_1,     Raphael_BR,  tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C2_2,     Leonardo_BL,  tmotorTetrix, openLoop)
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

//Used for reference by other functions (since #pragma configs are forced to be here)
//They're named after TMNT because Lisa.
typedef enum MecMotors {
	MecMotorFL = Donatello_FL,
	MecMotorBL = Leonardo_BL,
	MecMotorFR = Michelangelo_FR,
	MecMotorBR = Raphael_BR,
}

void initialize() {
	initServos();
	initMotors();
	servoSetAngle(Servo_Tube, 75);
}

task main() {
	initialize();
	waitForStart();
	while (true) {
		joyUpdateJoystickSettings();
		joymecdriveSetDesiredPowers(joyGetJoystickPointer());
		motorSetActualPowerToDesired();
	}
}
