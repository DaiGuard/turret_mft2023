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

        void fire();
        int damage();

    private:
        int _send_pin;
        int _recv_pin;
        int _no;
        
        const uint16_t _DATA_HIGH;
        const uint16_t _DATA_LOW;        

        uint16_t _rawData[67] =
            {
                9000, 4500, 650, // header
                1650, 600, 550, 650, 1650, 600, 550, 600, // 0xA
                550, 600, 1650, 650, 550, 600, 1650, 650, // 0x5
                550, 600, 550, 650, 550, 600, 550, 600,   // 0x0
                550, 600, 550, 650, 550, 600, 550, 600,   // 0x0            
                550, 600, 550, 650, 550, 600, 550, 600,    // 0x0
                550, 600, 550, 650, 550, 600, 550, 600,     // 0x0
                550, 600, 550, 650, 550, 600, 550, 600,  // 0x0
                550, 600, 550, 650, 550, 600, 550, 600  // 0x0
            };
};


#endif