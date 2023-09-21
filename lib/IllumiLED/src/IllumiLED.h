#ifndef __ILLUMI_LED_H__
#define __ILLUMI_LED_H__


#include <Adafruit_NeoPixel.h>


enum class IllumiPattern
{
    NONE        = 0x00,
    WAITCONNECT = 0x01, // 接続待機
    BOOTING     = 0x02, // 起動中
    IDLING      = 0x03, // 待機中
    DAMAGING    = 0x04, // ダメージ
    DOWN        = 0x05, // 戦闘不能
    FIRING      = 0x06, // 発射中
};


class IllumiLED: public Adafruit_NeoPixel
{
    public:
        IllumiLED(uint16_t n, int16_t p, neoPixelType t);
        ~IllumiLED(){}

        bool begin();
        void update();
        void switchLightingPattern(IllumiPattern pattern);

    private:
        int             _led_num;
        IllumiPattern   _pattern;
        uint64_t        _last_time;
        int             _count;

        uint8_t         _brightness;        
};

#endif