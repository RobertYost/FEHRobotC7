#ifndef LINEFOLLOWING_H
#define LINEFOLLOWING_H


class LineFollowing
{
public:
    LineFollowing();
    int DetermineStateOrange();
    int DetermineStateBlack();
    void DriveFindLineBlack(int motorPower);
    void DriveFindLineOrange();
    void DriveLine();
    void DriveLineOrange();
    void TurnLeftLine();
    void TurnLeftLineOrange();
    void TurnRightLine();
    void TurnRightLineOrange();
    void LineFollow(float time);
    void LineFollowOrange(float time);
private:
    void Reset();
};

extern LineFollowing LINEFOLLOWER;

#endif // LINEFOLLOWING_H
