#include "RobotSerial.h"
#include "CRC.h"


RobotSerial::RobotSerial()
    : SerialHeader(0xa5), SerialEnd(0x0a)
{
    _serial = NULL;
}


bool RobotSerial::begin(HardwareSerial* serial)
{
    _serial = serial;

    return true;
}


void RobotSerial::control(float* vec, uint16_t buttons)
{
    uint8_t buffer[18];

    // header + end write
    buffer[0] = SerialHeader;
    buffer[17] = SerialEnd;

    // float x 3 vec write
    memcpy(buffer+1, vec, sizeof(float)*3);

    // uint16_t buttons write
    memcpy(buffer+13, &buttons, sizeof(uint16_t));

    // crc write
    uint16_t crc = calcCRC16(buffer, 15, 0x8005, 0x0000, 0x0000, true, true);
    memcpy(buffer+15, &crc, sizeof(uint16_t));
    
    if(_serial->availableForWrite() >= 18)
    {
        _serial->write(buffer, 18);
    }
}