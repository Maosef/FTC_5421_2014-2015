#pragma config(Motor,  motorA,          singleMotor,   tmotorNXT, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//#include "Headers\Motor.h"
//#include "Headers\Servo.h"
#include "Headers\Data.h"
#include "JoystickDriver.c"

#include "Auton\Playback.h"

void initialize() {
	clearDebugStream();
	//motorInit();
	//servoInit();
}

task main()
{
	initialize();
	bool isEnd = false;
	waitForStart();
	//Joystick playback
	playbackRun();
}
