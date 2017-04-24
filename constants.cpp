#include "constants.h"

AnalogInputPin left(FEHIO::P3_0);
AnalogInputPin middle(FEHIO::P3_3);
AnalogInputPin right(FEHIO::P3_7);
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