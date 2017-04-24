#include "strings.h"
#include "constants.h"
#include "rpsnav.h"
#include "navigation.h"
#include "debugging.h"
#include "linefollowing.h"
#include <FEHRPS.h>

int GetCdSValue() {
    RPS.InitializeTouchMenu();

    float start = TimeNow();
    float startCellValue = cds_cell.Value();
    Sleep(2.0);
    while (cds_cell.Value() / startCellValue > 0.9) {
        int values[] = { cds_cell.Value() };
        DEBUGGER.PrintTelemetry(Strings::cds_cell, values);
    }

    // navigate to core deposit light, determine the color
    NAVIGATOR.Reverse(8.3);
    NAVIGATOR.TurnRight();
    NAVIGATOR.Reverse();
    NAVIGATOR.Drive(6.5);

    double time = TimeNow();
    int light_state = 0;
    while (TimeNow() - time < 1) {
        light_state = NAVIGATOR.ReadCdsCell();
    }

    return light_state;
}

void TurnSatellite() {
    NAVIGATOR.Drive(15.7);
    NAVIGATOR.TurnRight();
    NAVIGATOR.DriveForSatellite();
    NAVIGATOR.Reverse(10);
    NAVIGATOR.Reverse(30, 60);
    NAV.CheckHeading(280);
}

void HitSeismographButton() {
    NAVIGATOR.TurnLeft(75);
    NAV.CheckHeading(359);
    NAVIGATOR.DriveUntilWall();
    NAVIGATOR.Reverse(3);
    NAVIGATOR.TurnRight();
    NAVIGATOR.Reverse(26, 60);
    Sleep(5.0);
}

void PullLever() {
    NAVIGATOR.Drive(18.0);
    NAVIGATOR.TurnRight();
    NAVIGATOR.Reverse();
    NAVIGATOR.Drive(25, 50);
    Sleep(0.4);
    servo_arm.SetDegree(30);
    NAVIGATOR.Reverse(1);
    servo_arm.SetDegree(90);
    NAVIGATOR.Reverse(14);
    NAVIGATOR.TurnRight(23);
    NAV.CheckHeading(143);
}

bool PullCore(int light_state) {
    NAVIGATOR.Drive(13);

    bool leftBool, centerBool, rightBool;
    int count = 0;
    do {
        NAVIGATOR.TurnRight(1);
        NAVIGATOR.Drive(.05);
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
        NAVIGATOR.TurnLeft(1);
        NAVIGATOR.Drive(.05);
        centerBool = middle.Value() < ORANGE_MIDDLE_THRESHOLD;
        rightBool = right.Value() < ORANGE_MIDDLE_THRESHOLD;
        LCD.WriteRC(leftBool, 4, 5);
        LCD.WriteRC(centerBool, 5, 5);
        LCD.WriteRC(rightBool, 6, 5);
        LCD.WriteRC("loop end", 7, 5);
        count++;
    } while (!centerBool && !rightBool && count < 15);

    LINEFOLLOWER.LineFollowOrange(2);

    NAVIGATOR.Reverse(5, 20);
    servo_arm.SetDegree(30);
    LINEFOLLOWER.LineFollowOrange(1);
    servo_arm.SetDegree(45);
    for (int i = 0; i < 4; i++) {
        NAVIGATOR.ReverseSlantLeft(0.25, 25);
        NAVIGATOR.ReverseSlantRight(0.25, 25);
    }

    NAVIGATOR.Reverse(6);
    servo_arm.SetDegree(70);
    NAVIGATOR.TurnLeft(45);
    NAVIGATOR.Reverse();
    NAVIGATOR.Drive(3);
    NAVIGATOR.TurnLeft(80);
    NAV.CheckHeading(257);

    NAVIGATOR.Drive(100);
    NAVIGATOR.Reverse(2);

    NAVIGATOR.TurnRight();
    NAVIGATOR.Reverse();
    servo_arm.SetDegree(110);
    if (light_state == BLUE_STATE) {
        NAVIGATOR.Drive(18);
    } else if (light_state == RED_STATE) {
        NAVIGATOR.Drive(28.5);
    }
    NAVIGATOR.TurnLeft();
    NAVIGATOR.Drive(69);
    NAVIGATOR.Reverse(0.5);
    servo_arm.SetDegree(15);
    Sleep(1.0);
    NAVIGATOR.Reverse(1, 25);
    servo_arm.SetDegree(90);
    NAVIGATOR.Reverse(2, 30);
}

void GoHome(int light_state) {
    if (light_state == BLUE_STATE) {
        NAVIGATOR.TurnLeft(100);
        double start = TimeNow();
        left_motor.SetPercent(-35);
        right_motor.SetPercent(-50);
        NAVIGATOR.TurnLeft();
        NAVIGATOR.Drive(69);
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

int main(void) {
    servo_arm.SetMin(500);
    servo_arm.SetMax(2500);

    servo_arm.SetDegree(90);

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

    FullCourse();

    return 0;
}
