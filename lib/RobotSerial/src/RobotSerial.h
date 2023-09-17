#ifndef __ROBOT_SERIAL_H__
#define __ROBOT_SERIAL_H__
#include "Arduino.h"

typedef struct
{
   float direct[3];
   uint8_t buttons[2];
   uint16_t crc16;
}control_val;

typedef union {
  control_val val;
  uint8_t binary[16];
} control_cmd;


class RobotSerial
{
private:
    /* data */
    
public:
    RobotSerial(){};
    void control(control_cmd cmd);

};

#endif