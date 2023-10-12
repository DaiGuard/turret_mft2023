#ifndef __BLUETOOTH_PS4_H__
#define __BLUETOOTH_PS4_H__


#include <HID.h>

class BluetoothPS4: public HID
{
    public:
        BluetoothPS4(): HID(){}
        ~BluetoothPS4(){}

    bool begin();
    bool isConnected();    
    void read();
};


#endif