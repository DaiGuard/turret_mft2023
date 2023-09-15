#include <HpLED.h>


HpLED::HpLED(uint16_t n, int16_t p, neoPixelType t)
    : Adafruit_NeoPixel(n, p, t)
{
    _led_num = n;
    _hp = 100;
}


bool HpLED::begin()
{
    Adafruit_NeoPixel::begin();

    Adafruit_NeoPixel::clear();
    Adafruit_NeoPixel::setBrightness(50);
    for(int i=0; i<_led_num; i++)
    {
        Adafruit_NeoPixel::setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0));
    }

    Adafruit_NeoPixel::show();

    return true;
}

void HpLED::setHP(int hp)
{
    _hp = hp;

    int cursor = (int)(_hp / 100.0f * _led_num);

    uint32_t color = 0u;
    if(hp > 50)
    {
        color = Adafruit_NeoPixel::Color(0, 255, 0);
    }
    else if(hp > 25)
    {
        color = Adafruit_NeoPixel::Color(255, 255, 0);
    }
    else
    {
        color = Adafruit_NeoPixel::Color(255, 0, 0);
    }

    Adafruit_NeoPixel::clear();
    Adafruit_NeoPixel::setBrightness(50);
    for(int i=0; i<_led_num; i++)
    {
        if(i < cursor)
        {
            Adafruit_NeoPixel::setPixelColor(i, color);
        }
        else
        {
            Adafruit_NeoPixel::setPixelColor(i, 0u);
        }
    }

    Adafruit_NeoPixel::show();
}