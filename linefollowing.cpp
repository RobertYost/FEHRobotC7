#include "linefollowing.h"
#include "constants.h"
#include "debugging.h"
#include "strings.h"
#include "navigation.h"

LineFollowing LINEFOLLOWER;

LineFollowing::LineFollowing()
{

}

/**
 * @brief LineFollowing::Reset                      Stops the motors and resets the shaft encoding counts
 */
void LineFollowing::Reset() {
    left_motor.Stop();
    right_motor.Stop();
    left_encoder.ResetCounts();
    right_encoder.ResetCounts();
}

/**
 * @brief LineFollowing::DetermineStateBlack        Determines the relation of the robot to a black line on the course
 */
int LineFollowing::DetermineStateBlack() {
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

/**
 * @brief LineFollowing::DetermineStateOrange   Determines the relation of the robot to an orange line on the course
 */
int LineFollowing::DetermineStateOrange() {
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

/**
 * @brief LineFollowing::DriveFindLineBlack     Drives the robot forward at a specified motor power until a black line is detected
 * @param motorPower                            Motor power at which the robot drives forward
 */
void LineFollowing::DriveFindLineBlack(int motorPower) {
    Reset();
    float start = TimeNow();
    left_motor.SetPercent(MOTOR_CORRECTION * motorPower);
    right_motor.SetPercent(motorPower);
    while (left.Value() > BLACK_LEFT_THRESHOLD && right.Value() > BLACK_RIGHT_THRESHOLD && middle.Value() > BLACK_MIDDLE_THRESHOLD) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief LineFollowing::DriveFindLineOrange    Drives the robot foward until an orange line is detected
 */
void LineFollowing::DriveFindLineOrange() {
    Reset();
    for (int i = 0; i < 4; i++) {
        LCD.WriteRC("In Loop",7,1);
        float start = TimeNow();
        left_motor.SetPercent(15);
        right_motor.SetPercent(10);
        while (TimeNow() - start < 1) {
            if (left.Value() < ORANGE_LEFT_THRESHOLD || right.Value() < ORANGE_RIGHT_THRESHOLD || middle.Value() < ORANGE_MIDDLE_THRESHOLD) {
                Reset();
                return;
            }
        }
        Reset();
        start = TimeNow();
        right_motor.SetPercent(15);
        left_motor.SetPercent(10);
        while (TimeNow() - start < 1) {
            if (left.Value() < ORANGE_LEFT_THRESHOLD || right.Value() < ORANGE_RIGHT_THRESHOLD || middle.Value() < ORANGE_MIDDLE_THRESHOLD) {
                Reset();
                return;
            }
        }
        Reset();
    }
    Reset();
}

/**
 * @brief LineFollowing::DriveLine              Drives the robot along a black line
 */
void LineFollowing::DriveLine() {
    Reset();
    left_motor.SetPercent(MOTOR_CORRECTION * 20);
    right_motor.SetPercent(20);
    float start = TimeNow();
    while (DetermineStateBlack() == CENTER && TimeNow() - start < 1) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief LineFollowing::DriveLineOrange        Drives the robot along an orange line
 */
void LineFollowing::DriveLineOrange() {
    Reset();
    left_motor.SetPercent(MOTOR_CORRECTION * LINE_POWER);
    right_motor.SetPercent(LINE_POWER);
    float start = TimeNow();
    while (DetermineStateOrange() == CENTER && TimeNow() - start < 2) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief LineFollowing::TurnLeftLine           Turns the robot left along a black line
 */
void LineFollowing::TurnLeftLine() {
    Reset();
    left_motor.SetPercent(10);
    right_motor.SetPercent(0);
    float start = TimeNow();
    while (DetermineStateBlack() == RIGHT && TimeNow() - start < 1) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief LineFollowing::TurnLeftLineOrange     Turns the robot left along an orange line
 */
void LineFollowing::TurnLeftLineOrange() {
    Reset();
    right_motor.SetPercent(0);
    left_motor.SetPercent(15);
    float start = TimeNow();
    while (DetermineStateOrange() == RIGHT && TimeNow() - start < 2) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief LineFollowing::TurnRightLine          Turns the robot right along a black line
 */
void LineFollowing::TurnRightLine() {
    Reset();
    left_motor.SetPercent(0);
    right_motor.SetPercent(10);
    float start = TimeNow();
    while (DetermineStateBlack() == LEFT && TimeNow() - start < 1) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief LineFollowing::TurnRightLineOrange    Turns the robot right along an orange line
 */
void LineFollowing::TurnRightLineOrange() {
    Reset();
    left_motor.SetPercent(0);
    right_motor.SetPercent(15);
    float start = TimeNow();
    while (DetermineStateOrange() == LEFT && TimeNow() - start < 2) {
        int values[] = { left_encoder.Counts(), right_encoder.Counts() };
        DEBUGGER.PrintTelemetry(Strings::encoders, values);
    }
    Reset();
}

/**
 * @brief LineFollowing::LineFollow             Follows along a black line for a specified amount of time
 * @param time                                  Amount of time the robot should attempt to follow the line
 */
void LineFollowing::LineFollow(float time) {
    float start = TimeNow();
    while (TimeNow() - start < time && !NAVIGATOR.IsStall(696969420911))
    {
        int state = DetermineStateBlack();
        LCD.WriteLine(state);
        switch (state) {
        case CENTER:
            DriveLine();
            break;
        case LEFT:
            TurnRightLine();
            break;
        case RIGHT:
            TurnLeftLine();
            break;
        default:
            LCD.WriteRC("I'm going off the line, I am die. RIP", 4, 5);
            break;
        }
    }
}

/**
 * @brief LineFollowing::LineFollowOrange       Follows along an orange line for a specified amount of time
 * @param time                                  Amount of time the robot should attempt to follow the line
 */
void LineFollowing::LineFollowOrange(float time) {
    float start = TimeNow();
    while (TimeNow() - start < time)
    {
        int state = DetermineStateOrange();
        LCD.WriteRC(state, 1, 1);
        switch (state) {
        case CENTER:
            DriveLineOrange();
            break;
        case LEFT:
            TurnRightLineOrange();
            break;
        case RIGHT:
            TurnLeftLineOrange();
            break;
        default:
            LCD.WriteRC("I'm going off the line, I am die. RIP", 4, 5);
            break;
        }
    }
}
