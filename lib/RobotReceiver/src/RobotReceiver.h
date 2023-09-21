#ifndef __ROBOT_RECEIVER_H__
#define __ROBOT_RECEIVER_H__

#include <stdint.h>
#include <HardwareSerial.h>

typedef struct {
    float fb;
    float lr;
    float turn;

    bool up;
    bool right;
    bool down;
    bool left;

    bool square;
    bool cross;
    bool circle;
    bool triangle;
    
    bool l1;
    bool r1;
    bool l2;
    bool r2;    
} command_t;


class RobotReceiver
{
private:
    /* data */
    float cmd2float(uint8_t *data, int index);
    
    const uint8_t SerialHeader;
    const uint8_t SerialEnd;
    HardwareSerial* _serial;

public:
    RobotReceiver();
    bool begin(HardwareSerial* serial);
    bool update();

    command_t commands;
};

#endif