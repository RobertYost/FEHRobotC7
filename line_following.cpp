#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>

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

#define CENTER 0
#define RIGHT 1
#define LEFT 2


int determineState() {
   int state = CENTER;
   bool leftBool, centerBool, rightBool;
   leftBool = left.Value() > CURVED_LEFT_THRESHOLD;
   centerBool = middle.Value() > CURVED_MIDDLE_THRESHOLD;
   rightBool = right.Value() > CURVED_RIGHT_THRESHOLD;

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
   while (determineState() == LEFT);
}

void turnLeft() {
    reset();
    rightMotor.SetPercent(15);
    leftMotor.SetPercent(-15);
   while (determineState() == RIGHT);
}

void Drive() {
    reset();
    int power = 25;
    leftMotor.SetPercent(MOTOR_CORRECTION*power);
    rightMotor.SetPercent(power);
    while (determineState() == CENTER);
    reset();
}

int main(void)
{

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

    while (true) {
      int state = determineState();
      if (state == LEFT){
        turnRight();
      }
      else if (state == RIGHT){
        turnLeft();
      }
      else {
        Drive();
      }
    }


    return 0;
}
