#ifndef __HP_LED_H__
#define __HP_LED_H__


#include <Adafruit_NeoPixel.h>


class HpLED: public Adafruit_NeoPixel
{
    public:
        HpLED(uint16_t n, int16_t p, neoPixelType t);
        ~HpLED(){}

        bool begin();
        void setHP(int hp);

    private:
        int     _led_num;
        int     _hp;
};


#endif