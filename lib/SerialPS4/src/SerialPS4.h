#ifndef __SERIAL_PS4_H__
#define __SERIAL_PS4_H__

#include <HID.h>


class SerialPS4 : public HID
{
private:
    /* data */

public:
    bool isConnected(){return true;}
    bool begin(){return true;};
    void read();
};


#endif