#ifndef __SERIAL_PS4_H__
#define __SERIAL_PS4_H__

#include <HID.h>


class HardwareSerial;

class SerialPS4 : public HID
{
private:
    HardwareSerial* _serial;

public:
    SerialPS4(): HID(){}
    ~SerialPS4(){}

    /**
     * TODO: 受信を確認したら接続にするよう変更する
     */
    bool isConnected(){ return true; }
    bool begin();
    bool begin(HardwareSerial* serial);
    void read();
};


#endif