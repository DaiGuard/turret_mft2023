#ifndef __ROBOT_SERIAL_H__
#define __ROBOT_SERIAL_H__


#include <stdint.h>
#include <HardwareSerial.h>



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
    const uint8_t SerialHeader;
    const uint8_t SerialEnd;
    HardwareSerial* _serial;
    
public:
    RobotSerial();
    bool begin(HardwareSerial* serial);
    void control(float *vec, uint16_t buttons);

};

#endif