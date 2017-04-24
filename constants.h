#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHBattery.h>

const float BLACK_LEFT_THRESHOLD = 1.543;
const float BLACK_MIDDLE_THRESHOLD = 1.7003;
const float BLACK_RIGHT_THRESHOLD = 2.097;

const float ORANGE_LEFT_THRESHOLD = 1.300;
const float ORANGE_MIDDLE_THRESHOLD = 1.234;
const float ORANGE_RIGHT_THRESHOLD = 1.740;

const float DRIVE_CORRECTION = 0.91;
const float COUNTS_PER_INCH = 40.19;

const float MOTOR_CORRECTION = 0.99;
const int TURN_COUNT = 230;
const int TURN_COUNT_LEFT = 243;
const int TURNING_POWER = 25;
const int DEFAULT_MOTOR_POWER = 30;
const int EXTRA_DRIVE = 0;
const int LINE_POWER = 20;

const int INITIAL_TIMEOUT = 5;
const int DRIVE_TIMEOUT = 6;
const int STALL_COUNT = 50;

const int LEFT = 0;
const int CENTER = 1;
const int RIGHT = 2;

const float NO_LIGHT = 1.385;
const float BLUE_LIGHT = 1.000;
const float RED_LIGHT = 0.390;

const int NO_STATE = 0;
const int BLUE_STATE = 1;
const int RED_STATE = 2;

extern AnalogInputPin left;
extern AnalogInputPin middle;
extern AnalogInputPin right;

extern DigitalInputPin top_left_micro;
extern DigitalInputPin top_right_micro;
extern DigitalInputPin bottom_left_micro;
extern DigitalInputPin bottom_right_micro;

extern AnalogInputPin cds_cell;

extern FEHServo servo_arm;

extern DigitalEncoder left_encoder;
extern DigitalEncoder right_encoder;

extern FEHMotor left_motor;
extern FEHMotor right_motor;

#endif // CONSTANTS_H
