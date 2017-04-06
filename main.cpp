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

#define ORANGE_LEFT_THRESHOLD 2
#define ORANGE_MIDDLE_THRESHOLD 2
#define ORANGE_RIGHT_THRESHOLD 2

//normal 93
#define DRIVE_CORRECTION 0.93
#define COUNTS_PER_INCH 40.19

#define MOTOR_CORRECTION 0.99
#define TURN_COUNT 230
#define TURN_COUNT_LEFT 243
//normal 15,25
#define TURNING_POWER 25
#define DEFAULT_MOTOR_POWER 30
#define EXTRA_DRIVE 0
#define LINE_POWER 20

#define INITIAL_TIMEOUT 5
#define DRIVE_TIMEOUT 6
#define STALL_COUNT 50

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

bool isStall(float count) {
  bool leftStall = false;
  bool rightStall = false;

  int leftStartCount = left_encoder.Counts();
  int rightStartCount = right_encoder.Counts();
  //Wait 500 ms to reread encoder counts
  float start = TimeNow();
  while (TimeNow() - start < 0.3) {
      if (right_encoder.Counts() > count) {
          return false;
      }
  }
  int leftEndCount = left_encoder.Counts();
  int rightEndCount = right_encoder.Counts();
  if (rightEndCount - rightStartCount < STALL_COUNT) {
     rightStall = true;
     LCD.WriteRC("!!!-----R STALL------!!!",9,1);
  }
  if (leftEndCount - leftStartCount < STALL_COUNT) {
    leftStall = true;
    LCD.WriteRC("!!!-----L STALL------!!!",10,1);
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
    } else {
        LCD.Clear(FEHLCD::Blue);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteLine("BLUE");
        return BLUE_STATE;
    }
}

void reset() {
    left_motor.Stop();
    right_motor.Stop();
    left_encoder.ResetCounts();
    right_encoder.ResetCounts();
}

void turnLeft() {
    reset();
    right_motor.SetPercent(TURNING_POWER);
    left_motor.SetPercent(-1 * TURNING_POWER);
    while (right_encoder.Counts() < TURN_COUNT_LEFT && left_encoder.Counts() < TURN_COUNT_LEFT) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void turnLeft(float degrees) {
    reset();
    left_motor.SetPercent(-1 * TURNING_POWER);
    right_motor.SetPercent(TURNING_POWER);
    while (right_encoder.Counts() < (TURN_COUNT_LEFT / 90) * degrees && left_encoder.Counts() < (TURN_COUNT_LEFT / 90) * degrees) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void turnRight() {
    reset();
    left_motor.SetPercent(TURNING_POWER);
    right_motor.SetPercent(-1 * TURNING_POWER);
    while (right_encoder.Counts() < TURN_COUNT) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void turnRight(float degrees) {
    reset();
    left_motor.SetPercent(TURNING_POWER);
    right_motor.SetPercent(-1 * TURNING_POWER);
    while (right_encoder.Counts() < (TURN_COUNT / 90) * degrees) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void DriveForSatellite() {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION*25);
    right_motor.SetPercent(25);
    while ( RPS.SatellitePercent() < 90 && TimeNow() - start < DRIVE_TIMEOUT && !isStall(DRIVE_CORRECTION * COUNTS_PER_INCH * 30)) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
}

void Drive() {
    reset();
    left_motor.SetPercent(MOTOR_CORRECTION*(DEFAULT_MOTOR_POWER+EXTRA_DRIVE));
    right_motor.SetPercent(DEFAULT_MOTOR_POWER+EXTRA_DRIVE);
    //using large number for isStall since there is no set distance
    float start = TimeNow();
    while((top_right_micro.Value() || top_left_micro.Value()) && TimeNow() - start < DRIVE_TIMEOUT && !isStall(10000) ) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void DriveUntilWall() {
    reset();
    left_motor.SetPercent(MOTOR_CORRECTION*(DEFAULT_MOTOR_POWER+EXTRA_DRIVE));
    right_motor.SetPercent(DEFAULT_MOTOR_POWER+EXTRA_DRIVE);
    while((top_right_micro.Value() || top_left_micro.Value())) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void DriveSlantLeft() {
    reset();
    left_motor.SetPercent(MOTOR_CORRECTION*DEFAULT_MOTOR_POWER * 0.92);
    right_motor.SetPercent(DEFAULT_MOTOR_POWER);
    float start = TimeNow();
    //using large number for isStall since there is no set distance
    while((top_right_micro.Value() || top_left_micro.Value()) && TimeNow() - start < DRIVE_TIMEOUT && !isStall(10000)) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void DriveSlantRight(float inches) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION*DEFAULT_MOTOR_POWER*1.10);
    right_motor.SetPercent(DEFAULT_MOTOR_POWER);
    while(right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT && !isStall(DRIVE_CORRECTION * COUNTS_PER_INCH * inches)) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

/*
* Overloaded drive method to allow for no input (drive until hit wall), one input (inches), or two inputs (inches and power)
*/

void Drive(float inches) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION*DEFAULT_MOTOR_POWER);
    right_motor.SetPercent(DEFAULT_MOTOR_POWER);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT && !isStall(DRIVE_CORRECTION * COUNTS_PER_INCH * inches)) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void Drive(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(motorPower);
    while (right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT && !isStall(DRIVE_CORRECTION * COUNTS_PER_INCH * inches)) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void Reverse() {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1*MOTOR_CORRECTION*(DEFAULT_MOTOR_POWER+EXTRA_DRIVE));
    right_motor.SetPercent(-1*(DEFAULT_MOTOR_POWER+EXTRA_DRIVE));

    const char* titles[] = { "Left Encoder: ", "Right Encoder: " };
    while(bottom_right_micro.Value() || bottom_left_micro.Value() && TimeNow() - start < DRIVE_TIMEOUT) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void Reverse(int inches) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * DEFAULT_MOTOR_POWER);
    right_motor.SetPercent(-1 * DEFAULT_MOTOR_POWER);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT &&!isStall(DRIVE_CORRECTION * COUNTS_PER_INCH * inches)) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void Reverse(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * motorPower * MOTOR_CORRECTION);
    right_motor.SetPercent(-1 * motorPower);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void ReverseSlantLeft(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * motorPower * 0.9);
    right_motor.SetPercent(-1 * motorPower);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void ReverseSlantRight(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(-1 * motorPower * 0.9);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void DriveFindLineBlack(int motorPower){
    reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(motorPower);
    while (left.Value() > BLACK_LEFT_THRESHOLD && right.Value() > BLACK_RIGHT_THRESHOLD && middle.Value() > BLACK_MIDDLE_THRESHOLD) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    reset();
}

void DriveLine()
{
    left_motor.SetPercent(MOTOR_CORRECTION * 20);
    right_motor.SetPercent(20);
    float start = TimeNow();
    while (determineStateBlack() == CENTER && TimeNow() - start < 1) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    left_motor.Stop();
    right_motor.Stop();
}

void turnLeftLine()
{
    right_motor.SetPercent(0);
    left_motor.SetPercent(10);
    float start = TimeNow();
    while (determineStateBlack() == RIGHT && TimeNow() - start < 1) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    left_motor.Stop();
    right_motor.Stop();
}

void turnRightLine()
{
    left_motor.SetPercent(0);
    right_motor.SetPercent(10);
    float start = TimeNow();
    while (determineStateBlack() == LEFT && TimeNow() - start < 1) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    left_motor.Stop();
    right_motor.Stop();
}

void LineFollow(float time)
{
    float start = TimeNow();
    while (TimeNow() - start < time && !isStall(696969420911))
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

void DriveFindLineOrange(){
    reset();
    for (int i = 0; i < 4; i++){
        LCD.WriteRC("In Loop",7,1);
        float start = TimeNow();
        left_motor.SetPercent(15);
        right_motor.SetPercent(10);
        while (TimeNow() - start < 1){
            if (left.Value() < ORANGE_LEFT_THRESHOLD || right.Value() < ORANGE_RIGHT_THRESHOLD || middle.Value() < ORANGE_MIDDLE_THRESHOLD) {
                reset();
                return;
            }
        }
        reset();
        start = TimeNow();
        right_motor.SetPercent(15);
        left_motor.SetPercent(10);
        while (TimeNow() - start < 1){
            if (left.Value() < ORANGE_LEFT_THRESHOLD || right.Value() < ORANGE_RIGHT_THRESHOLD || middle.Value() < ORANGE_MIDDLE_THRESHOLD) {
                reset();
                return;
            }
        }
        reset();
    }
}

void DriveLineOrange()
{
    left_motor.SetPercent(MOTOR_CORRECTION * LINE_POWER);
    right_motor.SetPercent(LINE_POWER);
    float start = TimeNow();
    while (determineStateOrange() == CENTER && TimeNow() - start < 2) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    left_motor.Stop();
    right_motor.Stop();
}

void turnLeftLineOrange()
{
    right_motor.SetPercent(0);
    left_motor.SetPercent(15);
    float start = TimeNow();
    while (determineStateOrange() == RIGHT && TimeNow() - start < 2) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
    left_motor.Stop();
    right_motor.Stop();
}

void turnRightLineOrange()
{
    left_motor.SetPercent(0);
    right_motor.SetPercent(15);
    float start = TimeNow();
    while (determineStateOrange() == LEFT && TimeNow() - start < 2) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }
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
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
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
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) {
        LCD.WriteRC(left_encoder.Counts(), 4, 5);
        LCD.WriteRC(right_encoder.Counts(), 5, 5);
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void checkHeading(float heading) //using RPS
{
    while (std::abs(RPS.Heading() - heading) > 3) {
        if (RPS.Heading() > heading) {
            turn_right(SPEED, 10);
        } else {
            turn_left(SPEED, 10);
        }
    }
}

int GetCdSValue() {

    RPS.InitializeTouchMenu();

    float start = TimeNow();
    float startCellValue = cds_cell.Value();
    Sleep(2.0);
    while (cds_cell.Value() / startCellValue > 0.9) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
        if (TimeNow() - start < INITIAL_TIMEOUT) {
            LCD.WriteRC(cds_cell.Value(), 4, 5);
        }
    }

    // navigate to core deposit light, read the color
    Reverse(8.3);
    turnRight();
    Reverse();
    Drive(6.5);
    double time = TimeNow();
    int light_state = 0;
    while (TimeNow() - time < 1) {
        light_state = ReadCdsCell();
    }

    return light_state;
}

void TurnSatellite() {
    Drive(15.7);
    turnRight();
    DriveForSatellite();
    Reverse(10);
    Reverse(30, 60);
    checkHeading(280);
}

void HitSeismographButton() {
    turnLeft(75);
    checkHeading(359);
    DriveUntilWall();
    Reverse(2.5);
    turnRight();
    Reverse(26);
    Sleep(5.0);
}

void PullLever() {
    Drive(18.0);
    // TODO: RPS Check y position
    turnRight();
    Reverse();
    Drive(25);
    Sleep(0.25);
    servo_arm.SetDegree(30);
    Reverse(1);
    servo_arm.SetDegree(90);
    Reverse(14);
    turnRight(23); // 20
    checkHeading(143); // 140
}

bool PullCore(int light_state) {
    Drive(13);

    // find the line
    bool leftBool, centerBool, rightBool;
    int count = 0;
    do {
        turnRight(1);
        Drive(.05);
        leftBool = left.Value() < ORANGE_LEFT_THRESHOLD;
        centerBool = middle.Value() < ORANGE_MIDDLE_THRESHOLD;
        rightBool = right.Value() < ORANGE_MIDDLE_THRESHOLD;
        LCD.WriteRC(leftBool, 4, 5);
        LCD.WriteRC(centerBool, 5, 5);
        LCD.WriteRC(rightBool, 6, 5);
        LCD.WriteRC("loop end", 7, 5);
        count++;
    } while (!leftBool && !centerBool && !rightBool && count < 5);
    count = 0;
    do {
        turnLeft(1);
        Drive(.05);
        centerBool = middle.Value() < ORANGE_MIDDLE_THRESHOLD;
        rightBool = right.Value() < ORANGE_MIDDLE_THRESHOLD;
        LCD.WriteRC(leftBool, 4, 5);
        LCD.WriteRC(centerBool, 5, 5);
        LCD.WriteRC(rightBool, 6, 5);
        LCD.WriteRC("loop end", 7, 5);
        count++;
    } while (!centerBool && !rightBool && count < 15);


    LineFollowOrange(2);

    Reverse(5, 20);
    servo_arm.SetDegree(30);
    LineFollowOrange(1);
    servo_arm.SetDegree(45);
    for (int i = 0; i < 4; i ++) {
        ReverseSlantLeft(0.25,25);
        ReverseSlantRight(0.25,25);
    }
    Reverse(6);
    servo_arm.SetDegree(70);
    turnLeft(45);
    Reverse();
    Drive(3);
    turnLeft(80);
    checkHeading(257);

    Drive(100);
    Reverse(2);

    turnRight();
    Reverse();
    servo_arm.SetDegree(110);
    if (light_state == BLUE_STATE) {
        Drive(18);
    } else if (light_state == RED_STATE) {
        Drive(28.5);
    }
    turnLeft();
    Drive(69);
    Reverse(0.5);
    servo_arm.SetDegree(15);
    Sleep(1.0);
    Reverse(1,25);
    servo_arm.SetDegree(90);
    Reverse(2, 30);
}

void GoHome(int light_state) {
    if (light_state == BLUE_STATE) {
        turnLeft(100);
        double start = TimeNow();
        left_motor.SetPercent(-35);
        right_motor.SetPercent(-50);
        while (TimeNow() - start < 4.0) {
            LCD.WriteRC("PLACEHOLDER: COMMUNIST MANIFESTO", 1, 1);
        }
        turnLeft();
        Drive(69);
    } else {
        left_motor.SetPercent(-60);
        right_motor.SetPercent(-50);
    }
}


void FullCourse() {
    int light_state = GetCdSValue();
    TurnSatellite();
    HitSeismographButton();
    PullLever();
    PullCore(light_state);
    GoHome(light_state);
}

int main(void)
{
    servo_arm.SetMin(500);
    servo_arm.SetMax(2500);
    LCD.WriteLine("Starting Now");

    servo_arm.SetDegree(90);

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );
     FullCourse();
//    HitSeismographButton();

    return 0;
}
