#pragma config(Hubs,  S1, HTMotor,  HTMotor,  none,     none)
#pragma config(Sensor, S1,     ,               sensorI2CMuxController)
#pragma config(Motor,  mtr_S1_C2_2,     MotorFR,       tmotorTetrix, PIDControl)
#pragma config(Motor,  mtr_S1_C1_1,     MotorFL,       tmotorTetrix, PIDControl)
#pragma config(Motor,  mtr_S1_C1_2,     MotorBL,       tmotorTetrix, PIDControl)
#pragma config(Motor,  mtr_S1_C2_1,     MotorBR,       tmotorTetrix, PIDControl)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//RobotC joystick driver code
#include "JoystickDriver.c"

#define MAXSPEED 100
#define DEADBAND 10
#define MOD 1.0

//Enum to assign button numbers meaningful names
typedef enum JoyButtons {
	BUTTON_X = 1,
	BUTTON_A = 2,
	BUTTON_B = 3,
	BUTTON_Y = 4,
	BUTTON_LB = 5, //Left bumper
	BUTTON_RB = 6, //Right bumper
	BUTTON_LT = 7, //Left trigger
	BUTTON_RT = 8, //Right trigger
	BUTTON_BACK = 9,
	BUTTON_START = 10,
} JoyButtons;

//Enum to assign tophat ("d-pad") numbers meaningful names
//Uses compass directions
typedef enum JoyTophat {
	TOPHAT_N = 0,
	TOPHAT_NE = 1,
	TOPHAT_E = 2,
	TOPHAT_SE = 3,
	TOPHAT_S = 4,
	TOPHAT_SW = 5,
	TOPHAT_W = 6,
	TOPHAT_NW = 7,
} JoyTophat;

bool tophat(JoyTophat tophatBtn, TJoystick *joyState) {
	return ((joyState->joy1_TopHat & (1 << (tophatBtn))) != 0);
}

//Veery simple, just check if a button is pressed and then move in that direction.
//Child safe because you can only use one button at a time
void dpadDrive(TJoystick *joyState) {
	int speed = MAXSPEED;
	//Rotate CW if right trigger
	if(joy1Btn(BUTTON_RT)) {
		motor[MotorFL] = MAXSPEED;
		motor[MotorBL] = MAXSPEED;
		motor[MotorFR] = -MAXSPEED;
		motor[MotorBR] = -MAXSPEED;
	}
	//Rotate CCW if left trigger
	else if (joy1Btn(BUTTON_LT)) {
		motor[MotorFL] = -MAXSPEED;
		motor[MotorBL] = -MAXSPEED;
		motor[MotorFR] = MAXSPEED;
		motor[MotorBR] = MAXSPEED;
	}
	//Move forward if dpad up
	else if(tophat(TOPHAT_N, joyState)) {
		motor[MotorFL] = MAXSPEED;
		motor[MotorBL] = MAXSPEED;
		motor[MotorFR] = MAXSPEED;
		motor[MotorBR] = MAXSPEED;
	}
	//Move backward if dpad down
	else if(tophat(TOPHAT_S, joyState)) {
		motor[MotorFL] = -MAXSPEED;
		motor[MotorBL] = -MAXSPEED;
		motor[MotorFR] = -MAXSPEED;
		motor[MotorBR] = -MAXSPEED;
	}
	//Move left if dpad left
	else if(tophat(TOPHAT_W, joyState)) {
		motor[MotorFL] = -MAXSPEED;
		motor[MotorBL] = MAXSPEED;
		motor[MotorFR] = -MAXSPEED;
		motor[MotorBR] = MAXSPEED;
	}
	//Move right if dpad right
	else if(tophat(TOPHAT_E, joyState)) {
		motor[MotorFL] = MAXSPEED;
		motor[MotorBL] = -MAXSPEED;
		motor[MotorFR] = MAXSPEED;
		motor[MotorBR] = -MAXSPEED;
	} else {
		motor[MotorFL] = 0;
		motor[MotorBL] = 0;
		motor[MotorFR] = 0;
		motor[MotorBR] = 0;
	}
}

//Added the abs parts to fix driving
int joySpeed(int speed) {
	speed=speed/2;
	if (abs(speed) > MAXSPEED) {
		return MAXSPEED*(abs(speed)/speed);
	} else if (abs(speed) < DEADBAND) {
		return 0;
	} else {
		return speed;
	}
}

void joystickDrive(TJoystick *joyState) {
	int joy1y1 = joyState->joy1_y1;
	int joy1x1 = joyState->joy1_x1;
	int joy1x2 = joyState->joy1_x2;
	motor[MotorFL] = MOD*joySpeed(joy1y1 + joy1x2 + joy1x1);
	motor[MotorBL] = MOD*joySpeed(joy1y1 + joy1x2 - joy1x1);
	motor[MotorBR] = MOD*-1*joySpeed(joy1y1 - joy1x2 + joy1x1);
	motor[MotorFR] = MOD*-1*joySpeed(joy1y1 - joy1x2 - joy1x1);
}

task main()
{
	while(true) {
		getJoystickSettings(joystick);
		//*
		//if you feel brave use this (untested joystick code):
		joystickDrive(&joystick);
		//*/
		//dpadDrive(&joystick);
	}
}
