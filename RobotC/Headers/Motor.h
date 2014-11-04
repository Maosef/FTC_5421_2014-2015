#ifndef MOTOR_H
#define MOTOR_H
#pragma systemFile

#include "Helper.h"

////////////////////////////////////////////////////////////////////////////////////////
/*A note on array indexing:
Arrays can be accessed by converting the tMotor enum to an int to get the array
index. Here's the rationale for determining array size.
Although FTC caps the number of motors to 8 12v DC motors + 3 NXT motors,
RobotC caps it to kNumbOfTotalMotors. We have no idea how RobotC generates the tMotor
enum, so assume the worst and make the size of the array kNumbOfTotalMotors.
*/
////////////////////////////////////////////////////////////////////////////////////////

//Constants to keep PID working and counter stalling regardless of battery level
#define MAX_NEVEREST_POWER 78
#define MIN_NEVEREST_POWER 10
#define MAX_NORMAL_POWER 100
#define MIN_NORMAL_POWER 15

/*Used as reference point for max power. All external functions should use this and
  0 for minimum reference point. */
#define MAX_REFERENCE_POWER 100

//Slew rate for scaling values (amount to add per loop). Percentage.
#define MOTOR_SLEW_RATE 5

#define MAX_NUM_MOTORS ((int)kNumbOfTotalMotors)

//Struct for storing motor power limits for each motor
typedef struct MotorData {
	int maxPower;
	int minPower;
} MotorData;

//Array for storing MotorData for each motor
static MotorData motorDefinitions[MAX_NUM_MOTORS];

//Struct definition for desired motor powers/encoder lengths
typedef struct DesiredMotorVals {
	int power[MAX_NUM_MOTORS];
	int encoder[MAX_NUM_MOTORS];
} DesiredMotorVals;

//Enum for referencing mecanum motors
//They're named after TMNT because Lisa.
//TODO place in config file. this is forced to be here because pragmas can't be included...
//VOLATILE
typedef enum MecMotor {
	MecMotor_FL = Donatello_FL,
	MecMotor_BL = Leonardo_BL,
	MecMotor_FR = Michelangelo_FR,
	MecMotor_BR = Raphael_BR,
} MecMotor;

//Returns max reference power
int motorGetMaxReferencePower() {
	return MAX_REFERENCE_POWER;
}

//Flag for seeing if motor definitions have been initialized
bool motorDefsInitialized = false;

//Initialize motor definitions, preprocess slew rates
void motorInit() {
	//VOLATILE
	//init maxPower to MAX_NORMAL_POWER and minPower to MIN_NORMAL_POWER
	for (int i=0;i<MAX_NUM_MOTORS;i++) {
		motorDefinitions[i].maxPower = MAX_NORMAL_POWER;
		motorDefinitions[i].minPower = MIN_NORMAL_POWER;
	}
	//set drive max motor powers
	motorDefinitions[MecMotor_FL].maxPower = MAX_NEVEREST_POWER;
	motorDefinitions[MecMotor_BL].maxPower = MAX_NEVEREST_POWER;
	motorDefinitions[MecMotor_FR].maxPower = MAX_NEVEREST_POWER;
	motorDefinitions[MecMotor_BR].maxPower = MAX_NEVEREST_POWER;

	//set drive min motor powers
	motorDefinitions[MecMotor_FL].minPower = MIN_NEVEREST_POWER;
	motorDefinitions[MecMotor_BL].minPower = MIN_NEVEREST_POWER;
	motorDefinitions[MecMotor_FR].minPower = MIN_NEVEREST_POWER;
	motorDefinitions[MecMotor_BR].minPower = MIN_NEVEREST_POWER;

	motorDefsInitialized = true;
}

/*Private function, returns power value after scaling power
  to fit motor constraints. Expects values from 0 to
  motorGetMaxReferencePower().
  If definitions have not been initialized, debug and assume
  normal motor. */
static int motorScalePower(tMotor currentMotor, int power) {
	if (power != 0) { //Scaling assumes non zero power
		int maxPower;
		int minPower;
		if (!motorDefsInitialized) {
			//TODO DEBUG "assuming normal motor speeds!"
			maxPower = MAX_NORMAL_POWER;
			minPower = MIN_NORMAL_POWER;
		} else {
			maxPower = motorDefinitions[currentMotor].maxPower;
			minPower = motorDefinitions[currentMotor].minPower;
		}

		//Scale power
		float scaledPower = (((float)power / (float)motorGetMaxReferencePower()) *
												(float)(maxPower-minPower)) + (helpFindSign(power) * minPower);
	}
	return (int) power;
}

/*Bounds power to fit motor constraints.
	Does NOT scale power */
static int motorBoundPower(tMotor currentMotor, int power) {
	int boundPower = power;
	int minPower = motorDefinitions[currentMotor].minPower;
	int maxPower = motorDefinitions[currentMotor].maxPower;
	if (abs(power) < minPower) { //in deadband
		boundPower = 0;
	} else if (abs(power) > maxPower) {
		boundPower = helpFindSign(boundPower) * maxPower;
	}
	return boundPower;
}

//Update actual motor values with desired motor values
void motorSetActualPowerToDesired(DesiredMotorVals *desiredVals) {
	//Kludgey code, but it works
	//VOLATILE

	//Scale all powers
	int scaledFL = motorScalePower((tMotor)MecMotor_FL, desiredVals->power[MecMotor_FL]);
	int scaledBL = motorScalePower((tMotor)MecMotor_BL, desiredVals->power[MecMotor_BL]);
	int scaledFR = motorScalePower((tMotor)MecMotor_FR, desiredVals->power[MecMotor_FR]);
	int scaledBR = motorScalePower((tMotor)MecMotor_BR, desiredVals->power[MecMotor_BR]);
	int scaledSlewFL = motorScalePower((tMotor)MecMotor_FL, MOTOR_SLEW_RATE);
	int scaledSlewBL = motorScalePower((tMotor)MecMotor_BL, MOTOR_SLEW_RATE);
	int scaledSlewFR = motorScalePower((tMotor)MecMotor_FR, MOTOR_SLEW_RATE);
	int scaledSlewBR = motorScalePower((tMotor)MecMotor_BR, MOTOR_SLEW_RATE);

	writeDebugStreamLine("Desired: %d, %d, %d, %d", scaledFL, scaledBL, scaledFR, scaledBR);

	//Find differences between motor and desired
	int diffFL = scaledFL - motor[MecMotor_FL];
	int diffBL = scaledBL - motor[MecMotor_BL];
	int diffFR = scaledFR - motor[MecMotor_FR];
	int diffBR = scaledBR - motor[MecMotor_BR];

	//Determine whether to change by slew rate, or by jumping directly to desired speed (smaller is better)
	//Make sure to keep the sign of the change
	int rateFL = helpFindSign(diffFL) * (int)helpFindMinAbsFloat(diffFL, scaledSlewFL);
	int rateBL = helpFindSign(diffBL) * (int)helpFindMinAbsFloat(diffBL, scaledSlewBL);
	int rateFR = helpFindSign(diffFR) * (int)helpFindMinAbsFloat(diffFR, scaledSlewFR);
	int rateBR = helpFindSign(diffBR) * (int)helpFindMinAbsFloat(diffBR, scaledSlewBR);

	writeDebugStreamLine("FL: %d, BL: %d, FR: %d, BR: %d", motor[MecMotor_FL], motor[MecMotor_BL], motor[MecMotor_FR], motor[MecMotor_BR]);
	writeDebugStreamLine("Rate FL: %d", rateFL);
	writeDebugStreamLine("Rate BL: %d", rateBL);
	writeDebugStreamLine("Rate FR: %d", rateFR);
	writeDebugStreamLine("Rate BR: %d", rateBR);

	//Update motors
	motor[MecMotor_FL] = motor[MecMotor_FL] + rateFL;
	motor[MecMotor_BL] = motor[MecMotor_BL] + rateBL;
	motor[MecMotor_FR] = motor[MecMotor_FR] + rateFR;
	motor[MecMotor_BR] = motor[MecMotor_BR] + rateBR;
}

#endif
