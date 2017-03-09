#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHBattery.h>

#include <cstring>

AnalogInputPin left(FEHIO::P3_0);
AnalogInputPin middle(FEHIO::P3_7);
AnalogInputPin right(FEHIO::P3_4);
DigitalInputPin top_left_micro(FEHIO::P0_0);
DigitalInputPin top_right_micro(FEHIO::P0_2);
DigitalInputPin bottom_left_micro(FEHIO::P0_4);
DigitalInputPin bottom_right_micro(FEHIO::P0_6);

AnalogInputPin cds_cell(FEHIO::P0_7);

FEHServo servoArm(FEHServo::Servo0);

DigitalEncoder leftEncoder(FEHIO::P1_0);
DigitalEncoder rightEncoder(FEHIO::P1_1);

FEHMotor leftMotor( FEHMotor::Motor0, 12.0);
FEHMotor rightMotor( FEHMotor::Motor1, 12.0);

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
        LCD.WriteRC(cds_cell.Value(), 4, 5);
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

    }
    reset();
}

void Drive() {
    reset();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION*power);
    rightMotor.SetPercent(power);
    while(top_right_micro.Value() || top_left_micro.Value()) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void DriveSlantLeft() {
    reset();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION*power * 0.92);
    rightMotor.SetPercent(power);
    while(top_right_micro.Value() || top_left_micro.Value()) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void DriveSlantRight(float inches) {
    reset();
    int power = 25;
    float start = TimeNow();
    leftMotor.SetPercent(MOTOR_CORRECTION*power*1.10);
    rightMotor.SetPercent(power);
    while(rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
        LCD.WriteRC(cds_cell.Value(), 4, 5);
    }
    reset();
}

void Drive(float inches) {
    reset();
    float start = TimeNow();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION*power);
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
    while (rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10);
    reset();
}

void Reverse() {
    reset();
    float start = TimeNow();
    int power = 25;
    leftMotor.SetPercent(-1*MOTOR_CORRECTION*power);
    rightMotor.SetPercent(-1*power);

    const char* titles[] = { "Left Encoder: ", "Right Encoder: " };
    while(bottom_right_micro.Value() || bottom_left_micro.Value() && TimeNow() - start < 10) {
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
    }
    reset();
}

void Reverse(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    leftMotor.SetPercent(-1 * MOTOR_CORRECTION * motorPower);
    rightMotor.SetPercent(-1 * motorPower);
    while ( rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
    }
    reset();
}

void ReverseSlant(int inches, int motorPower) {
    reset();
    float start = TimeNow();
    leftMotor.SetPercent(-1 * MOTOR_CORRECTION * motorPower * 0.96);
    rightMotor.SetPercent(-1 * motorPower);
    while ( rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
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

void DriveLine()
{
    leftMotor.SetPercent(MOTOR_CORRECTION * 10);
    rightMotor.SetPercent(20);
    float start = TimeNow();
    while (determineState() == CENTER && TimeNow() - start < 1);
    leftMotor.Stop();
    rightMotor.Stop();
}

void turnLeftLine()
{
    rightMotor.SetPercent(0);
    leftMotor.SetPercent(10);
    float start = TimeNow();
    while (determineState() == RIGHT && TimeNow() - start < 1);
    leftMotor.Stop();
    rightMotor.Stop();
}

void turnRightLine()
{
    leftMotor.SetPercent(0);
    rightMotor.SetPercent(10);
    float start = TimeNow();
    while (determineState() == LEFT && TimeNow() - start < 1);
    leftMotor.Stop();
    rightMotor.Stop();
}

void LineFollow(float time)
{
    float start = TimeNow();
    while (TimeNow() - start < time)
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

void PT3() {
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
    Drive(6);
    double time = TimeNow();
    int light_state = 0;
    while (TimeNow() - time < 3) {
        light_state = ReadCdsCell();
    }
    LCD.Clear(FEHLCD::Black);





//    DriveSlantLeft();
    Drive(13);

    turnRight();
//    start = TimeNow();
//    while (TimeNow() - start < 5 && (top_left_micro.Value() && top_right_micro.Value())) {
//        Drive(1);
//    }
    Reverse(40,60);
    Reverse(20);



    Drive(4.5);
    turnRight();
    Reverse();


    servoArm.SetDegree(90);


    //Find line
    DriveSlantRight(14.5);
    DriveFindLine(15);
    LineFollow(10.0);
    Reverse(7,10);
    servoArm.SetDegree(15);
    LineFollow(4.0);
    servoArm.SetDegree(30);
    Reverse(10, 15);
    servoArm.SetDegree(90);
    turnLeft(30);
    Reverse();
    Drive(3);
    turnLeft();
    turnRight(10);
    start = TimeNow();
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
    servoArm.SetDegree(15);
    Sleep(1.0);
    servoArm.SetDegree(90);
    Reverse(10, 30);
}

int main(void)
{
    //servoArm.TouchCalibrate();
    servoArm.SetMin(500);
    servoArm.SetMax(2500);

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );
    PT3();
    return 0;
}
