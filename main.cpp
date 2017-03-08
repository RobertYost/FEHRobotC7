#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHRPS.h>
#include <FEHSD.h>
#include <cmath>

#include "rpsnav.h"

AnalogInputPin left(FEHIO::P3_0);
AnalogInputPin middle(FEHIO::P3_7);
AnalogInputPin right(FEHIO::P3_4);

DigitalInputPin top_left_micro(FEHIO::P0_0);
DigitalInputPin top_right_micro(FEHIO::P0_2);
DigitalInputPin bottom_left_micro(FEHIO::P0_4);
DigitalInputPin bottom_right_micro(FEHIO::P0_6);

AnalogInputPin cds_cell(FEHIO::P0_7);

DigitalEncoder leftEncoder(FEHIO::P1_0);
DigitalEncoder rightEncoder(FEHIO::P1_1);

FEHMotor leftMotor( FEHMotor::Motor0, 12.0);
FEHMotor rightMotor( FEHMotor::Motor1, 12.0);

FEHServo servoArm(FEHServo::Servo0);


#define BLACK_LEFT_THRESHOLD 1.543
#define BLACK_MIDDLE_THRESHOLD 1.7003
#define BLACK_RIGHT_THRESHOLD 2.097

#define ORANGE_LEFT_THRESHOLD 1.300
#define ORANGE_MIDDLE_THRESHOLD 1.234
#define ORANGE_RIGHT_THRESHOLD 1.740

#define DRIVE_CORRECTION 0.9523
#define COUNTS_PER_INCH 40.49
#define MOTOR_CORRECTION 0.90
#define TURN_COUNT 265
#define TURNING_POWER 15

#define INITIAL_TIMEOUT 30

#define CENTER 0
#define RIGHT 1
#define LEFT 2

#define NO_LIGHT 2.000
#define BLUE_LIGHT .500
#define RED_LIGHT 0.300

#define NO_LIGHT_STATE 10
#define BLUE_LIGHT_STATE 11
#define RED_LIGHT_STATE 12

int determineState() {
    int state = CENTER;
    bool leftBool, centerBool, rightBool;
    leftBool = left.Value() > ORANGE_LEFT_THRESHOLD;
    centerBool = middle.Value() > ORANGE_MIDDLE_THRESHOLD;
    rightBool = right.Value() > ORANGE_RIGHT_THRESHOLD;

    if (centerBool) {
        state = CENTER;
    } else {
        if (leftBool) {
            state = LEFT;
        }
        if (rightBool) {
            state = RIGHT;
        }
    }

    return state;
}

void DriveLine() {
    leftMotor.SetPercent(MOTOR_CORRECTION*10);
    rightMotor.SetPercent(10);
    float start = TimeNow();
    while (determineState() == CENTER && TimeNow() - start < 1);
    leftMotor.Stop();
    rightMotor.Stop();
}


void turnLeftLine() {
    rightMotor.SetPercent(0);
    leftMotor.SetPercent(10);
    float start = TimeNow();
    while (determineState() == RIGHT && TimeNow() - start < 1);
    leftMotor.Stop();
    rightMotor.Stop();
}

void turnRightLine() {
    leftMotor.SetPercent(0);
    rightMotor.SetPercent(10);
    float start = TimeNow();
    while (determineState() == LEFT && TimeNow() - start < 1);
    leftMotor.Stop();
    rightMotor.Stop();
}

void LineFollow(float time) {
    float start = TimeNow();
    while( TimeNow() - start < time )
        {
            int state = determineState();
            LCD.WriteLine(state);
            switch (state) {
            case CENTER:
                DriveLine();
                break;
            case LEFT:
                turnRightLine();
                break;
            case RIGHT:
                turnLeftLine();
                break;
            default:
                LCD.WriteRC("I'm going off the line, I am die. RIP", 4, 5);
                break;
            }
        }
}

void reset() {
    leftMotor.Stop();
    rightMotor.Stop();
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();
}

void turnLeft() {
    reset();
    rightMotor.SetPercent(TURNING_POWER);
    leftMotor.SetPercent(-1 * TURNING_POWER);
    while (leftEncoder.Counts() < 280 && rightEncoder.Counts() < 280) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void turnLeft(float degrees) {
    reset();
    leftMotor.SetPercent(-1 * TURNING_POWER);
    rightMotor.SetPercent(TURNING_POWER);
    while (leftEncoder.Counts() < (TURN_COUNT / 90) * degrees && rightEncoder.Counts() < (TURN_COUNT / 90) * degrees) {

    }
    reset();
}

void turnRight() {
    reset();
    leftMotor.SetPercent(TURNING_POWER);
    rightMotor.SetPercent(-1 * TURNING_POWER);
    while (leftEncoder.Counts() < TURN_COUNT && rightEncoder.Counts() < TURN_COUNT) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void turnRight(float degrees) {
    reset();
    leftMotor.SetPercent(TURNING_POWER);
    rightMotor.SetPercent(-1 * TURNING_POWER);
    while (leftEncoder.Counts() < (TURN_COUNT / 90) * degrees && rightEncoder.Counts() < (TURN_COUNT / 90) * degrees) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Drive() {
    reset();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION * power);
    rightMotor.SetPercent(power);
    while(top_right_micro.Value() || top_left_micro.Value()) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void DriveSlantLeft() {
    reset();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION * power * 0.95);
    rightMotor.SetPercent(power);
    while(top_right_micro.Value() || top_left_micro.Value()) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Drive(float inches) {
    reset();
    float start = TimeNow();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION * power);
    rightMotor.SetPercent(power);
    while ( rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Drive(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    leftMotor.SetPercent(MOTOR_CORRECTION * motorPower);
    rightMotor.SetPercent(motorPower);
    while (rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Reverse() {
    reset();
    float start = TimeNow();
    int power = 25;
    leftMotor.SetPercent(-1*MOTOR_CORRECTION*power);
    rightMotor.SetPercent(-1*power);
    while(bottom_right_micro.Value() || bottom_left_micro.Value() && TimeNow() - start < 10) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Reverse(int inches) {
    reset();
    float start = TimeNow();
    int power = 25;
    leftMotor.SetPercent(-1 * MOTOR_CORRECTION * power);
    rightMotor.SetPercent(-1 * power);
    while ( rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Reverse(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    leftMotor.SetPercent(-1 * MOTOR_CORRECTION * motorPower);
    rightMotor.SetPercent(-1 * motorPower);
    while ( rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void DriveFindLine(int motorPower){
    reset();
    float start = TimeNow();
    leftMotor.SetPercent(MOTOR_CORRECTION * motorPower);
    rightMotor.SetPercent(motorPower);
    while (left.Value() > ORANGE_LEFT_THRESHOLD && right.Value() > ORANGE_RIGHT_THRESHOLD && middle.Value() > ORANGE_MIDDLE_THRESHOLD);
    reset();
}

void PushButtonAndHitSwitch() {

    float start = TimeNow();
    Sleep(2.0);
    while (cds_cell.Value() > .7) {
        LCD.WriteLine(cds_cell.Value());
        if (TimeNow() - start > INITIAL_TIMEOUT) {
            LCD.WriteLine(cds_cell.Value());
        }
    }

    // Drive out from start
    Drive(10);
    // Back up into wall near start
    turnLeft();
    Reverse();
    // Drive until wall opposite from previous wall is reached
    Drive();
    // Backup and turn to drive up the mudslide incline
    Reverse(3);
    turnRight();
    Reverse(33, 40);
    // Turn right to face wall to the right
    turnRight();
    // Backup to wall
    Reverse();
//    // Drive out to line up with seismograph button
//    Drive(8);
//    turnLeft();
//    // Drive into seismograph button and wait
//    Reverse(20);
//    Sleep(7.0);
//    // Back out to line up with lava switch
//    Drive(18);
//    // Turn to face lava switch and run into it
//    turnRight();
    Drive(23);
    Sleep(2.0);
    Reverse(5);
}

int ReadCdsCell() {
    double value = cds_cell.Value();
    if (value < RED_LIGHT) {
        LCD.Clear(FEHLCD::Red);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteRC("RED", 3, 5);
        return RED_LIGHT_STATE;
    } else if (value < BLUE_LIGHT) {
        LCD.Clear(FEHLCD::Blue);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteRC("BLUE", 3, 5);
        return BLUE_LIGHT_STATE;
    } else {
        LCD.Clear(FEHLCD::Black);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteRC("NO LIGHT", 3, 5);
        return NO_LIGHT_STATE;
    }
}

void PullSwitchAndReadLight() {

    float start = TimeNow();
    Sleep(2.0);
    while (cds_cell.Value() > .7) {
        LCD.WriteLine(cds_cell.Value());
        if (TimeNow() - start > INITIAL_TIMEOUT) {
            LCD.WriteRC(cds_cell.Value(), 4, 5);
        }
    }

    Drive(12.25);
    turnLeft();
    Reverse();
    Drive(8.5);
    double time = TimeNow();
    int cds_state = 0;
    while (TimeNow() - time < 5) {
        cds_state = ReadCdsCell();
    }
    LCD.Clear(FEHLCD::Black);
    DriveSlantLeft();
    Reverse(1);
    turnRight();
    Reverse(34, 50);
    turnRight();
    Reverse();
    Drive(23.75);
    Reverse(5);
}



void PullCoreAndDeposit() {

    // Read light to start and make sure arm is upright
    servoArm.SetDegree(90);
    float start = TimeNow();
    Sleep(1.0);
    while (cds_cell.Value() > .7 && TimeNow() - start < INITIAL_TIMEOUT) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    LCD.Clear(FEHLCD::Black);
    // Back out and drive to the core deposit light, read it's color, and store it
    Reverse(10.25);
    turnRight();
    Reverse();
    Drive(6.5);
    float time = TimeNow();
    int light_state = 0;
    while (TimeNow() - time < 5) {
        light_state = ReadCdsCell();
    }
    LCD.Clear(FEHLCD::Black);
    // Drive to the wall nearest the mudslide ramp
    DriveSlantLeft();
    // Drive up the mudslide
    Reverse(1);
    turnRight();
    Reverse(40, 60);
    turnRight();
    Reverse();
    Drive(8);
    turnLeft();
    Reverse(25);
    turnRight();


    //TODO: Fix below code to reflect actual measurements
    //checkHeading(135); // How many ever degrees the robot may have to turn to line up with the core
    servoArm.SetDegree(90);
    DriveFindLine(10);
    LineFollow(6.0);
    Reverse(7,10);
    servoArm.SetDegree(15);
    LineFollow(4.0);
    servoArm.SetDegree(30);
    Reverse(10, 15);
    servoArm.SetDegree(90);
    turnLeft(30);
    Reverse();


    // TODO: Navigate back to deposit box
//    if (light_state == RED_LIGHT_STATE) {
//        reset();
//        turnLeft();
//        Drive(3);
//        turnRight();
//        Drive();
//        // TODO: Add servo logic to shake piece off of arm to scrape it off
//        Reverse(5, 10);
//    } else if (light_state == BLUE_LIGHT_STATE) {
//        reset();
//        turnRight();
//        Drive(3);
//        turnRight();
//        Drive();
//        // TODO: Add servo logic to shake piece off of arm to scrape it off
//        Reverse(5, 10);
//    }
    // TODO: Add logic to navigate to satellite and rotate it or touch it
    LCD.Clear(FEHLCD::Black);
    LCD.WriteRC("Satellite percent: ", 4, 5);
    LCD.WriteRC(RPS.SatellitePercent(), 4, 8);
    SD.Printf("Satellite percent: %f\n", RPS.SatellitePercent());
}

int main(void)
{

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

    //servoArm.TouchCalibrate();
    servoArm.SetMin(500);
    servoArm.SetMax(2500);

//    float x,y;
//    while (!LCD.Touch(&x, &y));
    PullCoreAndDeposit();

//    PushButtonAndHitSwitch();

    //PullSwitchAndReadLight();

//    while (true) {
//        LCD.WriteRC("Servo position: ", 3, 5);
//        LCD.WriteRC(servoArm._position, 4, 5);
//    }

    return 0;
}
