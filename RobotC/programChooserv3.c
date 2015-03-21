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

#include "Headers\Joystick.h"
#include "Headers\Motor.h"
#include "Headers\Servo.h"
#include "Headers\Global.h"
#include "Headers\Drive.h"
#include "AutonFxn\test.h"

//Stores desired motor values
DesiredMotorVals desiredMotorVals;
//Stores desired encoder values
DesiredEncVals desiredEncVals;

void initialize() {
	servoInit();
	motorInit(&desiredEncVals);
	//Initialize to zeroes
	memset(&desiredMotorVals, 0, sizeof(desiredMotorVals));
	memset(&desiredEncVals, 0, sizeof(desiredEncVals));
	clearDebugStream();
}

//define variables

int block = 0;
int onRamp = 0;
int teamDelay = 0;
int centerTube = 0;
int medTube = 0;

int startVar[5]; //startVar[0-4]
string autoActions[5]; //autoActions[0-4]

void programChooserInit(){
	startVar[0] = block;
	startVar[1] = onRamp;
	startVar[2] = teamDelay;
	startVar[3] = centerTube;
	startVar[4] = medTube;
	autoActions[0] = "block";
	autoActions[1] = "on ramp";
	autoActions[2] = "delay";
	autoActions[3] = "center";
	autoActions[4] = "medTube";
}

int changeState(int addVar,string action){
	int moveOn = 0;
	while(moveOn == 0){
		nxtDisplayString(2, "%s: %d", action,addVar);
		if (nNxtButtonPressed == 1){      //right arrow
					addVar++;
					addVar = addVar%2;
				}else if (nNxtButtonPressed == 2){ //left arrow
					addVar++;
					addVar = addVar%2;
				}
			if (nNxtButtonPressed == 3){  //orange button
				moveOn++;
			}
		}
		return addVar;
}

//Drive forward: int: 1 is true and 0 is false
//On Ramp: int: 1 is true and 0 is false
//delay: int: 1 is true and 0 is false

task main()
{
	programChooserInit();
	initialize();
	eraseDisplay();
	for(int i = 0; i<5;i++){
		startVar[i] = changeState(startVar[i], autoActions[i]);
		nxtDisplayClearTextLine(2);
		wait10Msec(50);
	}
	nxtDisplayString(2, "IT WORKS");
	//wait10Msec(5000);
	if (startVar[0] == 0){	// no block
		if (startVar[1] == 0){	//no block, not on ramp
			if (startVar[2] == 0){	//no block, not on ramp, no delay
				if (startVar[3] == 0){	//no block, not on ramp, no delay, no center tube
					if (startVar[4] == 0){	//no block, not on ramp, no delay, no center tube, no medium tube

					}else if(startVar[4] == 1){ //no block, not on ramp, no delay, no center tube, medium tube

					}
				}else if(startVar[3] == 1){
					if (startVar[4] == 0){	//no block, not on ramp, no delay, center tube, no medium tube

					}else if(startVar[4] == 1){ //no block, not on ramp, no delay, center tube, medium tube

					}
				}
			}else if(startVar[2] == 1){ //delay
				if (startVar[3] == 0){	//center tube
					if (startVar[4] == 0){	//no block, not on ramp, delay, no center tube, no medium tube

					}else if(startVar[4] == 1){//no block, not on ramp, delay, no center tube, medium tube

					}
				}else if(startVar[3] == 1){ //center tube
					if (startVar[4] == 0){	//no block, not on ramp, delay, center tube, no medium tube

					}else if(startVar[4] == 1){ //no block, not on ramp, delay, center tube, medium tube

					}
				}
			}
		}else if(startVar[1] == 1){ //onramp
			if (startVar[2] == 0){	//no block, on ramp, no delay
				if (startVar[3] == 0){	//no block, on ramp, no delay, no center tube
					if (startVar[4] == 0){	//no block, on ramp, no delay, no center tube, no medium tube

					}else if(startVar[4] == 1){ //no block, on ramp, no delay, no center tube, medium tube

					}
				}else if(startVar[3] == 1){
					if (startVar[4] == 0){	//no block, on ramp, no delay, center tube, no medium tube

					}else if(startVar[4] == 1){ //no block, on ramp, no delay, center tube, medium tube

					}
				}
			}else if(startVar[2] == 1){ //delay
				if (startVar[3] == 0){	//center tube
					if (startVar[4] == 0){	//no block, on ramp, delay, no center tube, no medium tube

					}else if(startVar[4] == 1){//no block, on ramp, delay, no center tube, medium tube

					}
				}else if(startVar[3] == 1){ //center tube
					if (startVar[4] == 0){	//no block, on ramp, delay, center tube, no medium tube

					}else if(startVar[4] == 1){ //no block, on ramp, delay, center tube, medium tube

					}
				}
			}
		}
	}else if(startVar[0] == 1){	//block
		if (startVar[1] == 0){	//block, not on ramp
			if (startVar[2] == 0){	//block, not on ramp, no delay
				if (startVar[3] == 0){	//block, not on ramp, no delay, no center tube
					if (startVar[4] == 0){	//block, not on ramp, no delay, no center tube, no medium tube

					}else if(startVar[4] == 1){ //block, not on ramp, no delay, no center tube, medium tube

					}
				}else if(startVar[3] == 1){
					if (startVar[4] == 0){	//block, not on ramp, no delay, center tube, no medium tube

					}else if(startVar[4] == 1){ //block, not on ramp, no delay, center tube, medium tube

					}
				}
			}else if(startVar[2] == 1){ //delay
				if (startVar[3] == 0){	//center tube
					if (startVar[4] == 0){	//block, not on ramp, delay, no center tube, no medium tube

					}else if(startVar[4] == 1){//block, not on ramp, delay, no center tube, medium tube

					}
				}else if(startVar[3] == 1){ //center tube
					if (startVar[4] == 0){	//block, not on ramp, delay, center tube, no medium tube

					}else if(startVar[4] == 1){ //block, not on ramp, delay, center tube, medium tube

					}
				}
			}
		}else if(startVar[1] == 1){ //onramp
			if (startVar[2] == 0){	//block, on ramp, no delay
				if (startVar[3] == 0){	//block, on ramp, no delay, no center tube
					if (startVar[4] == 0){	//block, on ramp, no delay, no center tube, no medium tube
						BlockCubix(&desiredMotorVals, &desiredEncVals);
					}else if(startVar[4] == 1){ //block, on ramp, no delay, no center tube, medium tube

					}
				}else if(startVar[3] == 1){
					if (startVar[4] == 0){	//block, on ramp, no delay, center tube, no medium tube

					}else if(startVar[4] == 1){ //block, on ramp, no delay, center tube, medium tube

					}
				}
			}else if(startVar[2] == 1){ //delay
				if (startVar[3] == 0){	//center tube
					if (startVar[4] == 0){	//block, on ramp, delay, no center tube, no medium tube

					}else if(startVar[4] == 1){//block, on ramp, delay, no center tube, medium tube

					}
				}else if(startVar[3] == 1){ //center tube
					if (startVar[4] == 0){	//block, on ramp, delay, center tube, no medium tube

					}else if(startVar[4] == 1){ //block, on ramp, delay, center tube, medium tube

					}
				}
			}
		}
	}
}
