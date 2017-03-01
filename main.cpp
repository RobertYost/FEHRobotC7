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

#define CURVED_LEFT_THRESHOLD 1.300
#define CURVED_MIDDLE_THRESHOLD 1.234
#define CURVED_RIGHT_THRESHOLD 1.740

#define DRIVE_CORRECTION 0.9523
#define COUNTS_PER_INCH 40.49
#define MOTOR_CORRECTION 0.90
#define TURN_COUNT 265
#define TURNING_POWER 15

#define INITIAL_TIMEOUT 30

#define CENTER 0
#define RIGHT 1
#define LEFT 2

#define NO_LIGHT 1.385
#define BLUE_LIGHT 1.0
#define RED_LIGHT 0.390

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
        LCD.WriteLine(cds_cell.Value());
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
        LCD.WriteLine(cds_cell.Value());
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
    leftMotor.SetPercent(MOTOR_CORRECTION * power);
    rightMotor.SetPercent(power);
    while(top_right_micro.Value() || top_left_micro.Value()) {
        LCD.WriteLine(cds_cell.Value());
    }
    reset();
}

void DriveSlantLeft() {
    reset();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION * power * 0.95);
    rightMotor.SetPercent(power);
    while(top_right_micro.Value() || top_left_micro.Value()) {
        LCD.WriteLine(cds_cell.Value());
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
        LCD.WriteLine(cds_cell.Value());
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
    while(bottom_right_micro.Value() || bottom_left_micro.Value() && TimeNow() - start < 10) {
        LCD.WriteLine(cds_cell.Value());
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

void ReadCdsCell() {
    double value = cds_cell.Value();
    if (value < RED_LIGHT) {
        LCD.Clear(FEHLCD::Red);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteLine("RED");
    } else if (value < BLUE_LIGHT) {
        LCD.Clear(FEHLCD::Blue);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteLine("BLUE");
    } else {
        LCD.Clear(FEHLCD::Black);
        LCD.SetFontColor(FEHLCD::White);
        LCD.WriteLine("NONE");
    }
}

void PullSwitchAndReadLight() {

    float start = TimeNow();
    Sleep(2.0);
    while (cds_cell.Value() > .7) {
        LCD.WriteLine(cds_cell.Value());
        if (TimeNow() - start > INITIAL_TIMEOUT) {
            LCD.WriteLine(cds_cell.Value());
        }
    }

    Drive(12.25);
    turnLeft();
    Reverse();
    Drive(8.5);
    double time = TimeNow();
    while (TimeNow() - time < 5) {
        ReadCdsCell();
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

int main(void)
{

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

//    PushButtonAndHitSwitch();

    PullSwitchAndReadLight();

    return 0;
}
