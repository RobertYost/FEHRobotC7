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
