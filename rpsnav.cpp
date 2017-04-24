#include "rpsnav.h"
#include "constants.h"
#include <FEHSD.h>
#include <FEHRPS.h>
#include <cmath>

#define SPEED 12

/*
 * Global variable to be called upon whenever this class is needed.
 */
RPSNav NAV;

RPSNav::RPSNav()
{
}

/**
 * @brief RPSNav::move_forward      moves the robot forward
 * @param percent                   motor percent at which the robot is run at
 * @param counts                    the number of shaft encoding counts the motors are rotated
 */
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
       SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
   }

   //Turn off motors
   right_motor.Stop();
   left_motor.Stop();
}

/**
 * @brief RPSNav::turn_left     rotates the robot counterclockwise
 * @param percent               motor percent at which the robot is run at
 * @param counts                the number of shaft encoding counts the motors are rotated
 */
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
       SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
   }

   //Turn off motors
   right_motor.Stop();
   left_motor.Stop();
}

/**
 * @brief RPSNav::turn_right    rotates the robot clockwise
 * @param percent               motor percent at which the robot is run at
 * @param counts                the number of shaft encoding counts the motors are rotated
 */
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
   while((left_encoder.Counts() + right_encoder.Counts()) / 2. < counts) {
       SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
   }

   //Turn off motors
   right_motor.Stop();
   left_motor.Stop();
}

/**
 * @brief RPSNav::checkHeading  checks to see if the robot is at a certain angle
 * @param heading               the desired heading to be rotated to
 */
void RPSNav::CheckHeading(float heading) //using RPS
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

/**
 * @brief RPSNav::check_x_minus  checks to see if the robot has reached a certain x coordinate while heading left
 * @param x_coordinate           the desired x coordinate to be reached
 */
void RPSNav::check_x_minus(float x_coordinate) //using RPS while robot is in the +x direction
{
   //check whether the robot is within an acceptable range
   while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
   {
       if(RPS.X() > x_coordinate)
       {
           //pulse the motors for a short duration in the correct direction
           move_forward(-SPEED, 5);
       }
       else if(RPS.X() < x_coordinate)
       {
           //pulse the motors for a short duration in the correct direction
           move_forward(SPEED, 5);
       }
       SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
   }
}

/**
 * @brief RPSNav::check_x_plus  checks to see if the robot has reached a certain x coordinate while heading right
 * @param x_coordinate          the desired x coordinate to be reached
 */
void RPSNav::check_x_plus(float x_coordinate) //using RPS while robot is in the +x direction
{
   //check whether the robot is within an acceptable range
   while(RPS.X() < x_coordinate - 1 || RPS.X() > x_coordinate + 1)
   {
       if(RPS.X() > x_coordinate)
       {
           //pulse the motors for a short duration in the correct direction
           move_forward(SPEED, 5);
       }
       else if(RPS.X() < x_coordinate)
       {
           //pulse the motors for a short duration in the correct direction
           move_forward(-SPEED, 5);
       }
       SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
   }
}

/**
 * @brief RPSNav::check_y_minus checks to see if the robot has reached a certain y coordinate while heading backwards
 * @param y_coordinate          the desired y coordinate to be reached
 */
void RPSNav::check_y_minus(float y_coordinate) //using RPS while robot is in the -y direction
{
   //check whether the robot is within an acceptable range
   while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
   {
       if(RPS.Y() > y_coordinate)
       {
           //pulse the motors for a short duration in the correct direction
           move_forward(SPEED, 5);
       }
       else if(RPS.Y() < y_coordinate)
       {
           //pulse the motors for a short duration in the correct direction
           move_forward(-SPEED, 5);
       }
       SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
   }
}

/**
 * @brief RPSNav::check_y_plus  checks to see if the robot has reached a certain y coordinate while heading forwards
 * @param y_coordinate          the desired y coordinate to be reached
 */
void RPSNav::check_y_plus(float y_coordinate) //using RPS while robot is in the +y direction
{
   //check whether the robot is within an acceptable range
   while(RPS.Y() < y_coordinate - 1 || RPS.Y() > y_coordinate + 1)
   {
       if(RPS.Y() > y_coordinate)
       {
           //pulse the motors for a short duration in the correct direction
           move_forward(-SPEED, 5);
       }
       else if(RPS.Y() < y_coordinate)
       {
           //pulse the motors for a short duration in the correct direction
           move_forward(SPEED, 5);
       }
       SD.Printf("%f, %f, %f\n", RPS.X(), RPS.Y(), RPS.Heading());
   }
}