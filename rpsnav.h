#ifndef RPSNAV_H
#define RPSNAV_H


class RPSNav
{
public:
    RPSNav();
    void checkHeading(float heading);
    void check_x_plus(float x_coordinate);
    void check_y_minus(float y_coordinate);
    void check_y_plus(float y_coordinate);
    void move_forward(int percent, int counts);
    void turn_left(int percent, int counts);
    void turn_right(int percent, int counts);
};

#endif // RPSNAV_H
