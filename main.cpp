#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>
#include <FEHUtility.h>

AnalogInputPin left(FEHIO::P3_0);
AnalogInputPin middle(FEHIO::P3_7);
AnalogInputPin right(FEHIO::P3_1);
DigitalInputPin top_left_micro(FEHIO::P0_0);
DigitalInputPin top_right_micro(FEHIO::P0_2);
DigitalInputPin bottom_left_micro(FEHIO::P0_4);
DigitalInputPin bottom_right_micro(FEHIO::P0_6);

AnalogInputPin cds_cell(FEHIO::P0_7);

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

#define DRIVE_CORRECTION 0.9523
#define COUNTS_PER_INCH 40.49
#define MOTOR_CORRECTION 0.96
#define TURN_COUNT 280

#define CENTER 0
#define RIGHT 1
#define LEFT 2

void reset () {
    leftMotor.Stop();
    rightMotor.Stop();
    leftEncoder.ResetCounts();
    rightEncoder.ResetCounts();
}

void turnRight() {
    reset();
    int power = 15;
    leftMotor.SetPercent(power);
    rightMotor.SetPercent(-1*power);
    while (leftEncoder.Counts() < TURN_COUNT && rightEncoder.Counts() < TURN_COUNT) {
        LCD.WriteLine("Turning right");
    }
    reset();
}

void turnLeft() {
    reset();
    int power = 15;
    rightMotor.SetPercent(power);
    leftMotor.SetPercent(-1*power);
    while (leftEncoder.Counts() < 280 && rightEncoder.Counts() < 280) {
        LCD.WriteLine("Turning left");
    }
    reset();
}

void Drive(int inches) {
    reset();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION*power);
    rightMotor.SetPercent(power);
    while ( rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches );
    reset();
}

void Reverse(int inches) {
    reset();
    int power = 25;
    leftMotor.SetPercent(-1*MOTOR_CORRECTION*power);
    rightMotor.SetPercent(-1*power);
    while ( rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches );
    reset();
}

void ReverseUntilWall() {
    reset();
    int power = 25;
    leftMotor.SetPercent(-1*MOTOR_CORRECTION*power);
    rightMotor.SetPercent(-1*power);
    while(bottom_right_micro.Value() && bottom_left_micro.Value());
    reset();
}

void ForwardUntilWall() {
    reset();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION*power);
    rightMotor.SetPercent(power);
    while(top_right_micro.Value() && top_left_micro.Value());
    reset();
}

void ReverseUphill(int inches) {
    reset();
    int power = 41;
    leftMotor.SetPercent(-1*MOTOR_CORRECTION*power);
    rightMotor.SetPercent(-1*power);
    while ( rightEncoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches );
    reset();
}

void PushButtonAndHitSwitch() {
    float start = TimeNow();
    while (cds_cell.Value() > .7 || TimeNow() - start < 30000);

    Drive(8);
    turnLeft();
    ReverseUntilWall();
    Drive(28);
    turnRight();
    ReverseUphill(35);
    turnRight();
    ReverseUntilWall();
    Drive(10);
    turnLeft();
    Reverse(20);
    Sleep(7.0);
    Drive(18);
    turnRight();
    Drive(12);
}

int main(void)
{

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

    PushButtonAndHitSwitch();

    return 0;
}
