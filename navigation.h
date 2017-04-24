#ifndef NAVIGATION_H
#define NAVIGATION_H


class Navigation
{
public:
    Navigation();
    int ReadCdsCell();
    bool IsStall(float count);
    void TurnLeft();
    void TurnLeft(float degrees);
    void TurnRight();
    void TurnRight(float degrees);
    void DriveForSatellite();
    void Drive();
    void Drive(float inches);
    void Drive(float inches, float motorPower);
    void DriveUntilWall();
    void DriveSlantLeft();
    void DriveSlantRight(float inches);
    void Reverse();
    void Reverse(float inches);
    void Reverse(float inches, float motorPower);
    void ReverseSlantLeft(float inches, float motorPower);
    void ReverseSlantRight(float inches, float motorPower);
private:
    void Reset();
};

// global instance of a Navigator object to be used wherever "navigation.h" is included
extern Navigation NAVIGATOR;

#endif // NAVIGATION_H
