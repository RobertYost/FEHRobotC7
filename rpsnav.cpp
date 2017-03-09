#include "rpsnav.h"
#include <FEHLCD.h>
#include <FEHIO.h>
#include <FEHRPS.h>
#include <FEHMotor.h>
#include <FEHUtility.h>
#include <FEHServo.h>
#include <FEHSD.h>
#include <cmath>

#define SPEED 10

DigitalEncoder left_encoder(FEHIO::P1_0);
DigitalEncoder right_encoder(FEHIO::P1_1);

FEHMotor left_motor( FEHMotor::Motor0, 12.0);
FEHMotor right_motor( FEHMotor::Motor1, 12.0);

RPSNav::RPSNav()
{
}

void RPSNav::move_forward(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder are less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) {
        SD.Printf("---Initial Values---");
        SD.Printf("X: %f\tY: %f\tHeading: %f", RPS.X(), RPS.Y(), RPS.Heading());
    }

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void RPSNav::turn_left(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(percent);
    left_motor.SetPercent(-percent);

    //While the average of the left and right encoder are less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) {

}

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}

void RPSNav::turn_right(int percent, int counts) //using encoders
{
    //Reset encoder counts
    right_encoder.ResetCounts();
    left_encoder.ResetCounts();

    //Set both motors to desired percent
    right_motor.SetPercent(-percent);
    left_motor.SetPercent(percent);

    //While the average of the left and right encoder are less than counts,
    //keep running motors
    while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts);

    //Turn off motors
    right_motor.Stop();
    left_motor.Stop();
}


void RPSNav::checkHeading(float heading) //using RPS
{
    //you will need to fill out this one yourself and take into account
    //the edge conditions (when you want the robot to go to 0 degrees
    //or close to 0 degrees)

    while (std::abs(RPS.Heading() - heading) > 4) {
        if (((int)(RPS.Heading() - heading + 360.0) % 360) > 180) {
            turn_right(10, 5);
        } else {
            turn_left(10, 5);
        }
        SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
    }
}

void RPSNav::check_x_plus(float x_coordinate) //using RPS while robot is in the +x direction
{
    //check whether the robot is within an acceptable range
    while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
    {
        if(RPS.X() > x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(-5);
//            right_motor.SetPercent(-5);
//            left_motor.Stop();
//            right_motor.Stop();
            move_forward(-SPEED, 5);
        }
        else if(RPS.X() < x_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(5);
//            right_motor.SetPercent(5);
//            left_motor.Stop();
//            right_motor.Stop();
            move_forward(SPEED, 5);
        }
    }
}

void RPSNav::check_y_minus(float y_coordinate) //using RPS while robot is in the -y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction
//            left_motor.SetPercent(-5);
//            right_motor.SetPercent(-5);
//            left_motor.Stop();
//            right_motor.Stop();
            move_forward(-SPEED, 5);


        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(5);
//            right_motor.SetPercent(5);
//            left_motor.Stop();
//            right_motor.Stop();
            move_forward(SPEED, 5);
        }
    }
}

void RPSNav::check_y_plus(float y_coordinate) //using RPS while robot is in the +y direction
{
    //check whether the robot is within an acceptable range
    while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
    {
        if(RPS.Y() > y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(-5);
//            right_motor.SetPercent(-5);
//            left_motor.Stop();
//            right_motor.Stop();
            move_forward(-SPEED, 5);
        }
        else if(RPS.Y() < y_coordinate)
        {
            //pulse the motors for a short duration in the correct direction

//            left_motor.SetPercent(5);
//            right_motor.SetPercent(5);
//            left_motor.Stop();
//            right_motor.Stop();
            move_forward(SPEED, 5);
        }
        SD.Printf("%f, %f, %f", RPS.X(), RPS.Y(), RPS.Heading());
    }
}
