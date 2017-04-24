#include "Navigation.h"

Navigation::Navigation()
{

}


//void Navigation::reset () {
//    left_motor.Stop();
//    right_motor.Stop();
//    left_encoder.ResetCounts();
//    right_encoder.ResetCounts();
//}

//void Navigation::turnLeft() {
//    reset();
//    right_motor.SetPercent(TURNING_POWER);
//    left_motor.SetPercent(-1 * TURNING_POWER);
//    while (left_encoder.Counts() < 280 && right_encoder.Counts() < 280) {
//        LCD.WriteRC(cds_cell.Value(), 4, 5);
//    }
//    reset();
//}

//void Navigation::turnLeft(float degrees) {
//    reset();
//    left_motor.SetPercent(-1 * TURNING_POWER);
//    right_motor.SetPercent(TURNING_POWER);
//    while (left_encoder.Counts() < (TURN_COUNT / 90) * degrees && right_encoder.Counts() < (TURN_COUNT / 90) * degrees) {
//        LCD.WriteRC(cds_cell.Value(), 4, 5);
//    }
//    reset();
//}

//void Navigation::turnRight() {
//    reset();
//    left_motor.SetPercent(TURNING_POWER);
//    right_motor.SetPercent(-1 * TURNING_POWER);
//    while (left_encoder.Counts() < TURN_COUNT && right_encoder.Counts() < TURN_COUNT) {
//        LCD.WriteRC(cds_cell.Value(), 4, 5);
//    }
//    reset();
//}

//void Navigation::turnRight(float degrees) {
//    reset();
//    left_motor.SetPercent(TURNING_POWER);
//    right_motor.SetPercent(-1 * TURNING_POWER);
//    while (left_encoder.Counts() < (TURN_COUNT / 90) * degrees && right_encoder.Counts() < (TURN_COUNT / 90) * degrees) {

//    }
//    reset();
//}

//void Drive() {
//    reset();
//    int power = 25;
//    left_motor.SetPercent(MOTOR_CORRECTION*power);
//    right_motor.SetPercent(power);
//    while(top_right_micro.Value() || top_left_micro.Value()) {
//        LCD.WriteRC(cds_cell.Value(), 4, 5);
//    }
//    reset();
//}

//void DriveSlantLeft() {
//    reset();
//    int power = 25;
//    left_motor.SetPercent(MOTOR_CORRECTION*power * 0.92);
//    right_motor.SetPercent(power);
//    while(top_right_micro.Value() || top_left_micro.Value()) {
//        LCD.WriteRC(cds_cell.Value(), 4, 5);
//    }
//    reset();
//}

//void DriveSlantRight(float inches) {
//    reset();
//    int power = 25;
//    float start = TimeNow();
//    left_motor.SetPercent(MOTOR_CORRECTION*power*1.10);
//    right_motor.SetPercent(power);
//    while(right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
//        LCD.WriteRC(cds_cell.Value(), 4, 5);
//    }
//    reset();
//}

//void Drive(float inches) {
//    reset();
//    float start = TimeNow();
//    int power = 25;
//    left_motor.SetPercent(MOTOR_CORRECTION*power);
//    right_motor.SetPercent(power);
//    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
//        LCD.WriteRC(cds_cell.Value(), 4, 5);
//    }
//    reset();
//}

//void Drive(int inches, int motorPower) {
//    reset();
//    float start = TimeNow();
//    left_motor.SetPercent(MOTOR_CORRECTION * motorPower);
//    right_motor.SetPercent(motorPower);
//    while (right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10);
//    reset();
//}

//void Reverse() {
//    reset();
//    float start = TimeNow();
//    int power = 25;
//    left_motor.SetPercent(-1*MOTOR_CORRECTION*power);
//    right_motor.SetPercent(-1*power);

//    const char* titles[] = { "Left Encoder: ", "Right Encoder: " };
//    while(bottom_right_micro.Value() || bottom_left_micro.Value() && TimeNow() - start < 10) {
//    }
//    reset();
//}

//void Reverse(int inches) {
//    reset();
//    float start = TimeNow();
//    int power = 25;
//    left_motor.SetPercent(-1 * MOTOR_CORRECTION * power);
//    right_motor.SetPercent(-1 * power);
//    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
//    }
//    reset();
//}

//void Reverse(int inches, int motorPower) {
//    reset();
//    float start = TimeNow();
//    left_motor.SetPercent(-1 * motorPower * MOTOR_CORRECTION);
//    right_motor.SetPercent(-1 * motorPower);
//    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < 10) {
//    }
//    reset();
//}
#include "navigation.h"
#include "constants.h"
#include "debugging.h"
#include "strings.h"
#include "FEHLCD.h"
#include <FEHRPS.h>

Navigation NAVIGATOR;

Navigation::Navigation() {

}

/**
 * @brief Navigation::ReadCdsCell   Reads the core deposit light value and determines the color of it
 * @return                          Color of the core deposit light
 */
int Navigation::ReadCdsCell() {
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

/**
 * @brief Navigation::Reset     Stops the motors and resets the shaft encoding counts
 */
void Navigation::Reset() {
    left_motor.Stop();
    right_motor.Stop();
    left_encoder.ResetCounts();
    right_encoder.ResetCounts();
}

/**
 * @brief Navigation::IsStall   Determines whether or not either of the motors have stalled
 * @param count                 The number of shaft encoder counts that the robot should take along its travels
 */
bool Navigation::IsStall(float count) {
    bool leftStall = false;
    bool rightStall = false;

    int leftStartCount = left_encoder.Counts();
    int rightStartCount = right_encoder.Counts();
    // Wait 500 ms to reread encoder counts
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
        LCD.WriteRC("!!!-----R STALL-----!!!", 9, 1);
    }
    if (leftEndCount - leftStartCount < STALL_COUNT) {
        leftStall = true;
        LCD.WriteRC("!!!-----L STALL-----!!!", 9, 1);
    }

    return leftStall && rightStall;
}

/**
 * @brief Navigation::TurnLeft  Rotates the robot counterclockwise 90 degrees
 */
void Navigation::TurnLeft() {
    Reset();
    left_motor.SetPercent(-1 * TURNING_POWER);
    right_motor.SetPercent(TURNING_POWER);
    while (right_encoder.Counts() < TURN_COUNT_LEFT && left_encoder.Counts() < TURN_COUNT_LEFT) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::TurnLeft  Rotates the robot counterclockwise a specified number of degrees
 * @param degrees               The number of degrees the robot should be rotated counterclockwise
 */
void Navigation::TurnLeft(float degrees) {
    Reset();
    left_motor.SetPercent(-1 * TURNING_POWER);
    right_motor.SetPercent(TURNING_POWER);
    while (right_encoder.Counts() < (TURN_COUNT_LEFT / 90) * degrees && left_encoder.Counts() < (TURN_COUNT_LEFT / 90) * degrees) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::TurnRight Rotates the robot clockwise 90 degrees
 */
void Navigation::TurnRight() {
    Reset();
    left_motor.SetPercent(TURNING_POWER);
    right_motor.SetPercent(-1 * TURNING_POWER);
    while (right_encoder.Counts() < TURN_COUNT) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::TurnRight Rotates the robot clockwise a specified number of degrees
 * @param degrees               The number of degrees the robot should be rotated clockwise
 */
void Navigation::TurnRight(float degrees) {
    Reset();
    left_motor.SetPercent(TURNING_POWER);
    right_motor.SetPercent(-1 * TURNING_POWER);
    while (right_encoder.Counts() < (TURN_COUNT / 90) * degrees) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::DriveForSatellite Drives the robot until the satellite is
 *                                      completely rotated or one of the motors stalls
 */
void Navigation::DriveForSatellite() {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION*25);
    right_motor.SetPercent(25);
    while ( RPS.SatellitePercent() < 90 && TimeNow() - start < DRIVE_TIMEOUT && !IsStall(DRIVE_CORRECTION * COUNTS_PER_INCH * 30)) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::Drive     Drives the robot forward until it triggers
 *                              one of the front microswitches or one of the motors stalls
 */
void Navigation::Drive() {
    Reset();
    left_motor.SetPercent(MOTOR_CORRECTION * (DEFAULT_MOTOR_POWER + EXTRA_DRIVE));
    right_motor.SetPercent(DEFAULT_MOTOR_POWER + EXTRA_DRIVE);
    //using large number for isStall since there is no set distance
    float start = TimeNow();
    while((top_right_micro.Value() || top_left_micro.Value()) && TimeNow() - start < DRIVE_TIMEOUT && !IsStall(10000) ) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::Drive     Drives the robot forward a specified distance
 * @param inches                The distance the robot should move forward
 */
void Navigation::Drive(float inches) {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION * DEFAULT_MOTOR_POWER);
    right_motor.SetPercent(DEFAULT_MOTOR_POWER);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT && !IsStall(DRIVE_CORRECTION * COUNTS_PER_INCH * inches)) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::Drive     Drives the robot forward a specified distance at a specified motor power
 * @param inches                The distance the robot should move forward
 * @param motorPower            The motor power at which the robot should operate
 */
void Navigation::Drive(float inches, float motorPower) {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(motorPower);
    while (right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT && !IsStall(DRIVE_CORRECTION * COUNTS_PER_INCH * inches)) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::DriveUntilWall    Drives the robot forward until one of the front microswitches are triggered
 */
void Navigation::DriveUntilWall() {
    Reset();
    left_motor.SetPercent(MOTOR_CORRECTION*(DEFAULT_MOTOR_POWER+EXTRA_DRIVE));
    right_motor.SetPercent(DEFAULT_MOTOR_POWER+EXTRA_DRIVE);
    while((top_right_micro.Value() || top_left_micro.Value())) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::DriveSlantLeft    Drives the robot forward with it slightly arcing leftward/counterclockwise
 */
void Navigation::DriveSlantLeft() {
    Reset();
    left_motor.SetPercent(MOTOR_CORRECTION*DEFAULT_MOTOR_POWER * 0.92);
    right_motor.SetPercent(DEFAULT_MOTOR_POWER);
    float start = TimeNow();
    //using large number for isStall since there is no set distance
    while((top_right_micro.Value() || top_left_micro.Value()) && TimeNow() - start < DRIVE_TIMEOUT && !IsStall(10000)) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::DriveSlantRight   Drives the robot forward a specified distance with it slightly arcing rightward/clockwise
 * @param inches                        Distance the robot should drive forward
 */
void Navigation::DriveSlantRight(float inches) {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION*DEFAULT_MOTOR_POWER*1.10);
    right_motor.SetPercent(DEFAULT_MOTOR_POWER);
    while(right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT && !IsStall(DRIVE_CORRECTION * COUNTS_PER_INCH * inches)) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::Reverse       Drives the robot in reverse until one of the back microswitches are triggered
 */
void Navigation::Reverse() {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(-1*MOTOR_CORRECTION*(DEFAULT_MOTOR_POWER+15));
    right_motor.SetPercent(-1*(DEFAULT_MOTOR_POWER+15));
    while(bottom_right_micro.Value() || bottom_left_micro.Value() && TimeNow() - start < DRIVE_TIMEOUT) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::Reverse       Drives the robot in reverse a specified distance
 * @param inches                    Distance the robot should drive in reverse
 */
void Navigation::Reverse(float inches) {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * DEFAULT_MOTOR_POWER);
    right_motor.SetPercent(-1 * DEFAULT_MOTOR_POWER);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT && !IsStall(DRIVE_CORRECTION * COUNTS_PER_INCH * inches)) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::Reverse       Drives the robot in reverse a specified distance at a specified motor power
 * @param inches                    Distance the robot should drive in reverse
 * @param motorPower                The motor power at which the robot should drive in reverse
 */
void Navigation::Reverse(float inches, float motorPower) {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * motorPower * MOTOR_CORRECTION);
    right_motor.SetPercent(-1 * motorPower);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::ReverseSlantLeft  Drives the robot a specified distance at a specified motor power
 *                                      in reverse slightly arcing leftward/clockwise
 * @param inches                        Distance the robot should drive in reverse
 * @param motorPower                    The motor power at which the robot should drive in reverse
 */
void Navigation::ReverseSlantLeft(float inches, float motorPower) {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * motorPower * 0.9);
    right_motor.SetPercent(-1 * motorPower);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief Navigation::ReverseSlantRight Drives the robot a specified distance at a specified motor
 *                                      in reverse slightly arcing rightward/counterclockwise
 * @param inches                        Distance the robot should drive in reverse
 * @param motorPower                    The motor power at which the robot should drive in reverse
 */
void Navigation::ReverseSlantRight(float inches, float motorPower) {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(-1 * MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(-1 * motorPower * 0.9);
    while ( right_encoder.Counts() <= DRIVE_CORRECTION * COUNTS_PER_INCH * inches && TimeNow() - start < DRIVE_TIMEOUT) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}
