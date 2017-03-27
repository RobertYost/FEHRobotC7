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

FEHServo servo_arm(FEHServo::Servo0);

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

bool isStall() {
  bool leftStall = false;
  bool rightStall = false;

  int leftStartCount = left_encoder.Counts();
  int rightStartCount = right_encoder.Counts();
  //Wait 500 ms to reread encoder counts
  Sleep(500);
  int leftEndCount = left_encoder.Counts();
  int rightEndCount = right_encoder.Counts();
  if (rightEndCount - rightStartCount < 3) {
    rightStall = true;
  }
  if (leftEndCount - leftStartCount < 3) {
    leftStall = true;
  }

  return leftStall && rightStall;
}

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
    while (right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 5);
    reset();
}

void Reverse() {
    reset();
    float start = TimeNow();
    int power = 25;
    left_motor.SetPercent(-1*MOTOR_CORRECTION*power);
    right_motor.SetPercent(-1*power);

    const char* titles[] = { "Left Encoder: ", "Right Encoder: " };
    while(bottom_right_micro.Value() || bottom_left_micro.Value() && TimeNow() - start < 5) {
    }
    reset();
}

void Reverse(int inches) {
    reset();
    float start = TimeNow();
    int power = 25;
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * power);
    right_motor.SetPercent(-1 * power);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 5) {
    }
    reset();
}

void Reverse(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * motorPower * MOTOR_CORRECTION);
    right_motor.SetPercent(-1 * motorPower - 2);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 5) {
    }
    reset();
}

void ReversePT3(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * motorPower * 0.90);
    right_motor.SetPercent(-1 * motorPower);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 5) {
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

    while (std::abs(RPS.Heading() - heading) > 3) {
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
    Drive(14.8);
    turnRight();
    while (RPS.SatellitePercent() < 90) {
        Drive(1);
    }
    Reverse(10);
    Reverse(35, 60);
    checkHeading(270);
}

void HitSeismographButton() {
    turnLeft();
    Drive();
    Reverse(4.5);
    turnRight();
    Reverse(18);
    Sleep(6.0);
}

void PullLever() {
    Drive(18.5);
    // TODO: RPS Check y position
    turnRight();
    Reverse();
//    checkHeading(180);
    Drive(25);
    servo_arm.SetDegree(30);
    Reverse(1);
    servo_arm.SetDegree(90);
    Reverse(16);
    turnRight(45);
    checkHeading(135);
}

void PullCore(int light_state) {
    Drive(15);
    DriveFindLineOrange(5);
    LineFollowOrange(4);

    Reverse(7, 10);
    servo_arm.SetDegree(20);
    Drive(10, 15);
    servo_arm.SetDegree(90);
    turnLeft(45);
    Reverse();
    Drive(3);
    turnLeft();
    turnRight(10);

    checkHeading(270);
    int start = TimeNow();
    while (TimeNow() - start < 10 && (top_left_micro.Value() && top_right_micro.Value())) {
        Drive(1);
    }
    Reverse(2);

    turnRight();
    Reverse();
    if (light_state == BLUE_STATE) {
        Drive(18);
    } else if (light_state == RED_STATE) {
        Drive(28);
    }
    turnLeft();
    LineFollow(5.0);
    servo_arm.SetDegree(15);
    Sleep(1.0);
    servo_arm.SetDegree(90);
    Reverse(10, 30);
}

void GoHome() {
    turnRight();
    Drive();
    Reverse(1);
    turnRight();
    Drive(7);
}


//void PT3() {
//    float start = TimeNow();
//    Sleep(2.0);
//    while (cds_cell.Value() > .7) {
//        LCD.WriteRC(cds_cell.Value(), 4, 5);
//        if (TimeNow() - start < INITIAL_TIMEOUT) {
//            LCD.WriteRC(cds_cell.Value(), 4, 5);
//        }
//    }
//    Reverse(9.5);
//    turnRight();
//    Reverse();
//    Drive(6);
//    double time = TimeNow();
//    int light_state = 0;
//    while (TimeNow() - time < 3) {
//        light_state = ReadCdsCell();
//    }
//    LCD.Clear(FEHLCD::Black);





////    DriveSlantLeft();
//    Drive(13);

//    turnRight();
////    start = TimeNow();
////    while (TimeNow() - start < 5 && (top_left_micro.Value() && top_right_micro.Value())) {
////        Drive(1);
////    }
//    Reverse(40,60);
//    Reverse(20);



//    Drive(4.5);
//    turnRight();
//    Reverse();


//    servoArm.SetDegree(90);


//    //Find line
//    DriveSlantRight(14.5);
//    DriveFindLine(15);
//    LineFollow(10.0);
//    Reverse(7,10);
//    servoArm.SetDegree(15);
//    LineFollow(4.0);
//    servoArm.SetDegree(30);
//    Reverse(10, 15);
//    servoArm.SetDegree(90);
//    turnLeft(30);
//    Reverse();
//    Drive(3);
//    turnLeft();
//    turnRight(10);
//    start = TimeNow();
//    while (TimeNow() - start < 10 && (top_left_micro.Value() && top_right_micro.Value())) {
//        Drive(1);
//    }
//    Reverse(2);

//    turnRight();
//    Reverse();
//    if (light_state == BLUE_STATE) {
//        Drive(18);
//    } else if (light_state == RED_STATE) {
//        Drive(28);
//    }
//    turnLeft();
//    LineFollow(5.0);
//    servoArm.SetDegree(15);
//    Sleep(1.0);
//    servoArm.SetDegree(90);
//    Reverse(10, 30);
//}


void FullCourse() {
    int light_state = GetCdSValue();
    TurnSatellite();
    HitSeismographButton();
    PullLever();
    PullCore(light_state);
    GoHome();
}

int main(void)
{
    servo_arm.SetMin(500);
    servo_arm.SetMax(2500);

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

    FullCourse();

    return 0;
}
