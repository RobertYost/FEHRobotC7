#ifndef RPSNAV_H
#define RPSNAV_H


class RPSNav
{
public:
    RPSNav();
    void CheckHeading(float heading);
    void check_x_minus(float x_coordinate);
    void check_x_plus(float x_coordinate);
    void check_y_minus(float y_coordinate);
    void check_y_plus(float y_coordinate);
private:
    void move_forward(int percent, int counts);
    void turn_left(int percent, int counts);
    void turn_right(int percent, int counts);
};

// global instance of an RPSNav object to be used wherever "rpsnav.h" is included
extern RPSNav NAV;

#endif // RPSNAV_H
