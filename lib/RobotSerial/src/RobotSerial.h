#ifndef __ROBOT_SERIAL_H__
#define __ROBOT_SERIAL_H__
#include "Arduino.h"


class RobotSerial
{
private:
    /* data */
    // float cmd2float(uint8_t *data, int index);
    

public:
    RobotSerial(){};
    void control(float* direct, bool* buttons);

};

#endif