#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>
#include <FEHUtility.h>
#include <FEHRPS.h>
#include <FEHSD.h>
#include <FEHServo.h>
#include <FEHBattery.h>

#include <cstring>
#include <cmath>

#include "strings.h"
#include "debugging.h"

AnalogInputPin left(FEHIO::P3_3);
AnalogInputPin middle(FEHIO::P3_7);
AnalogInputPin right(FEHIO::P3_0);
DigitalInputPin top_left_micro(FEHIO::P0_5);
DigitalInputPin top_right_micro(FEHIO::P2_1);
DigitalInputPin bottom_left_micro(FEHIO::P1_6);
DigitalInputPin bottom_right_micro(FEHIO::P2_4);

AnalogInputPin cds_cell(FEHIO::P1_0);

FEHServo servoArm(FEHServo::Servo0);

DigitalEncoder left_encoder(FEHIO::P0_2);
DigitalEncoder right_encoder(FEHIO::P0_0);

FEHMotor left_motor( FEHMotor::Motor1, 12.0);
FEHMotor right_motor( FEHMotor::Motor0, 12.0);

#define BLACK_LEFT_THRESHOLD 1.543
#define BLACK_MIDDLE_THRESHOLD 1.7003
#define BLACK_RIGHT_THRESHOLD 2.097

#define ORANGE_LEFT_THRESHOLD 1.300
#define ORANGE_MIDDLE_THRESHOLD 1.234
#define ORANGE_RIGHT_THRESHOLD 1.740

#define DRIVE_CORRECTION 0.9523
#define COUNTS_PER_INCH 40.49
#define MOTOR_CORRECTION 0.95
#define TURN_COUNT 265
#define TURNING_POWER 15

#define INITIAL_TIMEOUT 5

#define CENTER 0
#define RIGHT 1
#define LEFT 2

#define NO_LIGHT 1.385
#define BLUE_LIGHT 1.0
#define RED_LIGHT 0.390

#define BLUE_STATE 0
#define RED_STATE 1
#define NO_STATE 2

#define SPEED 12

void PrintTelemetry(const char* titles[], bool values[]) {
    LCD.WriteRC("TELEMETRY VALUES", 1, 4);
    LCD.WriteRC("Battery: ", 3, 3);
    LCD.WriteRC((int)(Battery.Voltage() / 11.7 * 100), 3, 12);
    for(int i = 0; i < sizeof(values); i++) {
        LCD.WriteRC(titles[i], 4 + i, 3);
        LCD.WriteRC(values[i], 4 + i, 3 + std::strlen(titles[i]));
    }
}

void PrintTelemetry(const char* titles[], int values[]) {
    LCD.WriteRC("TELEMETRY VALUES", 1, 4);
    LCD.WriteRC("Battery: ", 3, 3);
    LCD.WriteRC((int)(Battery.Voltage() / 11.7 * 100), 3, 12);
    for(int i = 0; i < sizeof(values); i++) {
        LCD.WriteRC(titles[i], i + 4, 3);
        LCD.WriteRC(values[i], i + 4, 3 + std::strlen(titles[i]));
    }
}

void PrintTelemetry(const char* titles[], float values[]) {
    LCD.WriteRC("TELEMETRY VALUES", 1, 4);
    LCD.WriteRC("Battery: ", 3, 3);
    LCD.WriteRC((int)(Battery.Voltage() / 11.7 * 100), 3, 12);
    for(unsigned int i = 0; i < sizeof(values); i++) {
        LCD.WriteRC(titles[i], 4 + i, 3);
        LCD.WriteRC(values[i], 4 + i, 3 + std::strlen(titles[i]));
    }
}

int determineStateOrange() {
   int state = CENTER;
   bool leftBool, centerBool, rightBool;
   leftBool = left.Value() < ORANGE_LEFT_THRESHOLD;
   centerBool = middle.Value() < ORANGE_MIDDLE_THRESHOLD;
   rightBool = right.Value() < ORANGE_RIGHT_THRESHOLD;

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

int determineStateBlack() {
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


int ReadCdsCell() {
    double value = cds_cell.Value();
    if (value < RED_LIGHT) {
        LCD.Clear(FEHLCD::Red);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteLine("RED");
        return RED_STATE;
    } else if (value < BLUE_LIGHT) {
        LCD.Clear(FEHLCD::Blue);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteLine("BLUE");
        return BLUE_STATE;
    } else {
        LCD.Clear(FEHLCD::Black);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteLine("NONE");
        return NO_STATE;
    }
}

void reset () {
    left_motor.Stop();
    right_motor.Stop();
    left_encoder.ResetCounts();
    right_encoder.ResetCounts();
}

void turnLeft() {
    reset();
    right_motor.SetPercent(TURNING_POWER);
    left_motor.SetPercent(-1 * TURNING_POWER);
    while (left_encoder.Counts() < 280 && right_encoder.Counts() < 280) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void turnLeft(float degrees) {
    reset();
    left_motor.SetPercent(-1 * TURNING_POWER);
    right_motor.SetPercent(TURNING_POWER);
    while (left_encoder.Counts() < (TURN_COUNT / 90) * degrees && right_encoder.Counts() < (TURN_COUNT / 90) * degrees) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void turnRight() {
    reset();
    left_motor.SetPercent(TURNING_POWER);
    right_motor.SetPercent(-1 * TURNING_POWER);
    while (left_encoder.Counts() < TURN_COUNT && right_encoder.Counts() < TURN_COUNT) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void turnRight(float degrees) {
    reset();
    left_motor.SetPercent(TURNING_POWER);
    right_motor.SetPercent(-1 * TURNING_POWER);
    while (left_encoder.Counts() < (TURN_COUNT / 90) * degrees && right_encoder.Counts() < (TURN_COUNT / 90) * degrees) {

    }
    reset();
}

void Drive() {
    reset();
    int power = 25;
    left_motor.SetPercent(MOTOR_CORRECTION*power);
    right_motor.SetPercent(power);
    while(top_right_micro.Value() || top_left_micro.Value()) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void DriveSlantLeft() {
    reset();
    int power = 25;
    left_motor.SetPercent(MOTOR_CORRECTION*power * 0.92);
    right_motor.SetPercent(power);
    while(top_right_micro.Value() || top_left_micro.Value()) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void DriveSlantRight(float inches) {
    reset();
    int power = 25;
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION*power*1.10);
    right_motor.SetPercent(power);
    while(right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Drive(float inches) {
    reset();
    float start = TimeNow();
    int power = 25;
    left_motor.SetPercent(MOTOR_CORRECTION*power);
    right_motor.SetPercent(power);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Drive(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(motorPower);
    while (right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10);
    reset();
}

void Reverse() {
    reset();
    float start = TimeNow();
    int power = 25;
    left_motor.SetPercent(-1*MOTOR_CORRECTION*power);
    right_motor.SetPercent(-1*power);

    const char* titles[] = { "Left Encoder: ", "Right Encoder: " };
    while(bottom_right_micro.Value() || bottom_left_micro.Value() && TimeNow() - start < 10) {
    }
    reset();
}

void Reverse(int inches) {
    reset();
    float start = TimeNow();
    int power = 25;
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * power);
    right_motor.SetPercent(-1 * power);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
    }
    reset();
}

void Reverse(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * motorPower * MOTOR_CORRECTION);
    right_motor.SetPercent(-1 * motorPower);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
    }
    reset();
}

void ReversePT3(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * motorPower * 0.90);
    right_motor.SetPercent(-1 * motorPower);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
    }
    reset();
}

void ReverseSlant(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * motorPower * 0.96);
    right_motor.SetPercent(-1 * motorPower);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
    }
    reset();
}

void DriveFindLineBlack(int motorPower){
    reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(motorPower);
    while (left.Value() > BLACK_LEFT_THRESHOLD && right.Value() > BLACK_RIGHT_THRESHOLD && middle.Value() > BLACK_MIDDLE_THRESHOLD);
    reset();
}

void DriveLine()
{
    left_motor.SetPercent(MOTOR_CORRECTION * 20);
    right_motor.SetPercent(20);
    float start = TimeNow();
    while (determineStateBlack() == CENTER && TimeNow() - start < 1);
    left_motor.Stop();
    right_motor.Stop();
}

void turnLeftLine()
{
    right_motor.SetPercent(0);
    left_motor.SetPercent(10);
    float start = TimeNow();
    while (determineStateBlack() == RIGHT && TimeNow() - start < 1);
    left_motor.Stop();
    right_motor.Stop();
}

void turnRightLine()
{
    left_motor.SetPercent(0);
    right_motor.SetPercent(10);
    float start = TimeNow();
    while (determineStateBlack() == LEFT && TimeNow() - start < 1);
    left_motor.Stop();
    right_motor.Stop();
}

void LineFollow(float time)
{
    float start = TimeNow();
    while (TimeNow() - start < time)
    {
        int state = determineStateBlack();
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



void DriveFindLineOrange(int motorPower){
    reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(motorPower);
    while (left.Value() > ORANGE_LEFT_THRESHOLD && right.Value() > ORANGE_RIGHT_THRESHOLD && middle.Value() > ORANGE_MIDDLE_THRESHOLD);
    reset();
}


void DriveLineOrange()
{
    left_motor.SetPercent(MOTOR_CORRECTION * 15);
    right_motor.SetPercent(15);
    float start = TimeNow();
    while (determineStateOrange() == CENTER && TimeNow() - start < 2);
    left_motor.Stop();
    right_motor.Stop();
}

void turnLeftLineOrange()
{
    right_motor.SetPercent(0);
    left_motor.SetPercent(15);
    float start = TimeNow();
    while (determineStateOrange() == RIGHT && TimeNow() - start < 2);
    left_motor.Stop();
    right_motor.Stop();
}

void turnRightLineOrange()
{
    left_motor.SetPercent(0);
    right_motor.SetPercent(15);
    float start = TimeNow();
    while (determineStateOrange() == LEFT && TimeNow() - start < 2);
    left_motor.Stop();
    right_motor.Stop();
}

void LineFollowOrange(float time)
{
    float start = TimeNow();
    while (TimeNow() - start < time)
    {
        int state = determineStateOrange();
        LCD.WriteRC(state, 1, 1);
        switch (state) {
        case CENTER:
            DriveLineOrange();
            break;
        case LEFT:
            turnRightLineOrange();
            break;
        case RIGHT:
            turnLeftLineOrange();
            break;
        default:
            LCD.WriteRC("I'm going off the line, I am die. RIP", 4, 5);
            break;
        }
    }
}

void turn_left(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder are less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) {

}

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void turn_right(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder are less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

//void checkHeading(float heading) //using RPS
//{
//    //you will need to fill out this one yourself and take into account
//    //the edge conditions (when you want the robot to go to 0 degrees
//    //or close to 0 degrees)

//    while (std::abs(RPS.Heading() - heading) > 10) {
//        if (((int)(RPS.Heading() - heading + 360.0) % 360) > 180) {
//            right_motor.SetPercent(-SPEED);
//            left_motor.SetPercent(SPEED);
//            Sleep(10);
//            right_motor.Stop();
//            left_motor.Stop();
//        } else {
//            right_motor.SetPercent(SPEED);
//            left_motor.SetPercent(-SPEED);
//            Sleep(10);
//            right_motor.Stop();
//            left_motor.Stop();
//        }
//        SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
//    }
//}

void checkHeading(float heading) //using RPS
{
    //you will need to fill out this one yourself and take into account
    //the edge conditions (when you want the robot to go to 0 degrees
    //or close to 0 degrees)

//    while (RPS.Heading() < heading - 1 || RPS.Heading() > heading - 1)
//    {
//        if (RPS.Heading() > heading) {
//            turn_right(SPEED, 5);
//        }
//        if (RPS.Heading() < heading) {
//            turn_left(SPEED, 5);
//        }
//        if (RPS.Heading() == 0) {
//            turn_left(SPEED, 5);
//        }
//    }

    while (std::abs(RPS.Heading() - heading) > 4) {
        if (RPS.Heading() > heading) {
            turn_right(SPEED, 10);
        } else {
            turn_left(SPEED, 10);
        }
    }
}

//void check_x_plus(float x_coordinate) //using RPS while robot is in the +x direction
//{
//    //check whether the robot is within an acceptable range
//    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
//    {
//        if(RPS.X() > x_coordinate)
//        {
//            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(-5);
//            right_motor.SetPercent(-5);
//            left_motor.Stop();
//            right_motor.Stop();
//            move_forward(-SPEED, 5);
//        }
//        else if(RPS.X() < x_coordinate)
//        {
//            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(5);
//            right_motor.SetPercent(5);
//            left_motor.Stop();
//            right_motor.Stop();
//            move_forward(SPEED, 5);
//        }
//    }
//}

//void check_y_minus(float y_coordinate) //using RPS while robot is in the -y direction
//{
//    //check whether the robot is within an acceptable range
//    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
//    {
//        if(RPS.Y() > y_coordinate)
//        {
//            //pulse the motors for a short duration in the correct direction
//            left_motor.SetPercent(-5);
//            right_motor.SetPercent(-5);
//            left_motor.Stop();
//            right_motor.Stop();
//            move_forward(-SPEED, 5);


//        }
//        else if(RPS.Y() < y_coordinate)
//        {
//            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(5);
//            right_motor.SetPercent(5);
//            left_motor.Stop();
//            right_motor.Stop();
//            move_forward(SPEED, 5);
//        }
//    }
//}

//void check_y_plus(float y_coordinate) //using RPS while robot is in the +y direction
//{
//    //check whether the robot is within an acceptable range
//    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
//    {
//        if(RPS.Y() > y_coordinate)
//        {
//            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(-5);
//            right_motor.SetPercent(-5);
//            left_motor.Stop();
//            right_motor.Stop();
//            move_forward(-SPEED, 5);
//        }
//        else if(RPS.Y() < y_coordinate)
//        {
//            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(5);
//            right_motor.SetPercent(5);
//            left_motor.Stop();
//            right_motor.Stop();
//            move_forward(SPEED, 5);
//        }
//        SD.Printf("%f, %f, %f", RPS.X(), RPS.Y(), RPS.Heading());
//    }
//}




void PT4() {
    // detect light to start

    RPS.InitializeTouchMenu();

    float start = TimeNow();
    Sleep(2.0);
    while (cds_cell.Value() > .7) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
        if (TimeNow() - start < INITIAL_TIMEOUT) {
            LCD.WriteRC(cds_cell.Value(), 4, 5);
        }
    }

    Reverse(9.5);
    turnRight();
    Reverse();
    Drive(21.5);
    turnRight();
    while (RPS.SatellitePercent() < 90) {
        Drive(1);
    }
    Reverse(10);
    Reverse(40, 60);
    checkHeading(270);
    Drive(35);
    turnRight();
    Reverse();
    Drive();
    Reverse(1);
    turnRight();
    Drive();
}

int GetCdSValue() {

    RPS.InitializeTouchMenu();
    servoArm.SetDegree(90);

    float start = TimeNow();
    Sleep(2.0);
    while (cds_cell.Value() > .7) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
        if (TimeNow() - start < INITIAL_TIMEOUT) {
            LCD.WriteRC(cds_cell.Value(), 4, 5);
        }
    }

    // navigate to core deposit light, read the color
        Reverse(9.5);
        turnRight();
        Reverse();
        Drive(6.5);
        double time = TimeNow();
        int light_state = 0;
        while (TimeNow() - time < 3) {
            light_state = ReadCdsCell();
        }

    return light_state;
}

void TurnSatellite() {
    Drive(15);
    turnRight();
    while (RPS.SatellitePercent() < 90) {
        Drive(1);
    }
    Reverse(10);
    Reverse(40, 60);
    checkHeading(90);
}

void HitSeismographButton() {
    turnRight();
    Drive();
    Reverse(2);
    turnLeft();
    checkHeading(270);
    Reverse(20);
    Sleep(7.0);
}

void PullLever() {
    Drive(30);
    checkHeading(180);
}

void FullCourse() {
    int light_state = GetCdSValue();
    TurnSatellite();
    HitSeismographButton();
}

int main(void)
{
    servoArm.SetMin(500);
    servoArm.SetMax(2500);

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );
//    FullCourse();
    while (true) {
////        float values[] = { left.Value(), middle.Value(), right.Value() };
////        PrintTelemetry(Strings::optosensors, values);
        LCD.WriteRC(left.Value(), 4, 5);
        LCD.WriteRC(middle.Value(), 5, 5);
        LCD.WriteRC(right.Value(), 6, 5);
    }


    return 0;
}
