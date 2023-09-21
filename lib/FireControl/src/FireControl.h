#ifndef __FIRE_CONTROL_H__
#define __FIRE_CONTROL_H__

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <stdint.h>


class FireControl:
    public IRrecv, IRsend
{
    public:
        FireControl(int send_pin, int recv_pin, uint8_t no);
        ~FireControl(){}

        bool begin();

        bool fire();
        int damage();

    private:
        int _send_pin;
        int _recv_pin;
        int _no;

        uint64_t _last_time;
        
        const uint16_t _DATA_HIGH;
        const uint16_t _DATA_LOW;        

        uint16_t _rawData[66] =
            {
                // 9000, 4500, 650, // header
                9000, 4500, // header
                1650, 650, 550, 650, 1650, 650, 550, 650, // 0xA
                550, 650, 1650, 650, 550, 650, 1650, 650, // 0x5
                1650, 650, 550, 650, 1650, 650, 1650, 650,   // 0xb
                550, 650, 1650, 650, 1650, 650, 550, 650,   // 0x6
                550, 650, 1650, 650, 550, 650, 550, 650,    // 0x4
                550, 650, 550, 650, 550, 650, 550, 650,     // 0x0
                1650, 650, 550, 650, 1650, 650, 1650, 650,  // 0xb
                1650,650, 1650, 650, 1650, 650, 1650, 650  // 0xf
            };
};


#endif