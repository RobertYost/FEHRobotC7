#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>

AnalogInputPin left(FEHIO::P3_0);
AnalogInputPin middle(FEHIO::P3_7);
AnalogInputPin right(FEHIO::P3_1);

AnalogInputPin top_left_micro(FEHIO::P0_0);
AnalogInputPin top_right_micro(FEHIO::P0_2);
AnalogInputPin bottom_left_micro(FEHIO::P0_4);
AnalogInputPin bottom_right_micro(FEHIO::P0_6);


DigitalEncoder leftEncoder(FEHIO::P1_0);
DigitalEncoder rightEncoder(FEHIO::P1_1);

FEHMotor leftMotor( FEHMotor::Motor0, 12.0);
FEHMotor rightMotor( FEHMotor::Motor1, 12.0);

#define STRAIGHT_LEFT_THRESHOLD 1.543
#define STRAIGHT_MIDDLE_THRESHOLD 1.7003
#define STRAIGHT_RIGHT_THRESHOLD 2.097

#define CURVED_LEFT_THRESHOLD 1.275
#define CURVED_MIDDLE_THRESHOLD 1.4175
#define CURVED_RIGHT_THRESHOLD 1.491

#define CENTER 0
#define RIGHT 1
#define LEFT 2


//int determineState() {
//    int state = CENTER;
//    bool leftBool, centerBool, rightBool;
//    leftBool = left.Value() > CURVED_LEFT_THRESHOLD;
//    centerBool = middle.Value() > CURVED_MIDDLE_THRESHOLD;
//    rightBool = right.Value() > CURVED_RIGHT_THRESHOLD;

//    if (centerBool) {
//        state = CENTER;
//    } else {
//        if (leftBool) {
//            state = LEFT;
//        }
//        if (rightBool) {
//            state = RIGHT;
//        }
//    }

//    return state;
//}

void reset () {
    leftMotor.Stop();
    rightMotor.Stop();
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();
}

void turnRight() {
    reset();
    leftMotor.SetPercent(15);
    rightMotor.SetPercent(-15);
    while (leftEncoder.Counts() < 223 && rightEncoder.Counts() < 223) {
        LCD.WriteLine("Turning right");
    }
//    while (determineState() == LEFT);
}

void turnLeft() {
    reset();
    rightMotor.SetPercent(15);
    leftMotor.SetPercent(-15);
    while (leftEncoder.Counts() < 223 && rightEncoder.Counts() < 223) {
        LCD.WriteLine("Turning left");
    }
//    while (determineState() == RIGHT);
}

void Drive(int inches) {
    reset();
    leftMotor.SetPercent(25);
    rightMotor.SetPercent(25);
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2 <= 200 * inches ) {
        LCD.Write("LEFT: ");
        LCD.WriteLine(leftEncoder.Counts());
        LCD.Write("Right: ");
        LCD.WriteLine(rightEncoder.Counts());
    }
    reset();
}

int main(void)
{

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

//    Drive(5);
//    turnLeft();
//    Drive(7);
//    turnLeft();
//    Drive(30);
//    turnRight();
//    Drive(5);
//    turnLeft();
//    Drive(8);
    while (true) {
//        LCD.Write("Top Left: ");
//        LCD.WriteLine(top_left_micro.Value());
//        LCD.Write("Top Right: ");
//        LCD.WriteLine(top_right_micro.Value());
//        LCD.Write("Bottom Left: ");
//        LCD.WriteLine(bottom_left_micro.Value());
//        LCD.Write("Bottom Right: ");
//        LCD.WriteLine(bottom_right_micro.Value());
//        Sleep(1.0);
        LCD.Write("Left: ");
        LCD.WriteLine(left.Value());
        LCD.Write("Middle: ");
        LCD.WriteLine(middle.Value());
        LCD.Write("Right: ");
        LCD.WriteLine(right.Value());
        Sleep(1.0);
    }

    return 0;
}
