#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHMotor.h>

AnalogInputPin left(FEHIO::P1_2);
AnalogInputPin middle(FEHIO::P1_1);
AnalogInputPin right(FEHIO::P1_0);

DigitalEncoder leftEncoder(FEHIO::P0_1);
DigitalEncoder rightEncoder(FEHIO::P0_0);

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

void Drive(int inches) {
    reset();
    leftMotor.SetPercent(25);
    rightMotor.SetPercent(25);
    while ((leftEncoder.Counts() + rightEncoder.Counts()) / 2 <= 200 * inches );
}

int main(void)
{

    LCD.Clear( FEHLCD::Black );
    LCD.SetFontColor( FEHLCD::White );

    while( true )
    {

    }
    return 0;
}
