#include <IllumiLED.h>



IllumiLED::IllumiLED(uint16_t n, int16_t p, neoPixelType t)
    : Adafruit_NeoPixel(n, p, t)
{
    _led_num = n;
    _last_time = 0lu;
    _pattern = IllumiPattern::WAITCONNECT;
    _brightness = 0;
}


bool IllumiLED::begin()
{
    Adafruit_NeoPixel::begin();
    Adafruit_NeoPixel::show();

    return true;
}


void IllumiLED::update()
{
    uint64_t start_time = millis();
    uint64_t dt = start_time - _last_time;
    float rate = 0.0;
    int cursor = 0;

    Adafruit_NeoPixel::clear();

    switch(_pattern)
    {
        case IllumiPattern::WAITCONNECT:            
            if(dt < 1500)
            {
                rate = (float)dt / 1500.0f;
            }
            else if(dt < 2200)
            {
                rate = 1.0f - (float)(dt - 1500)/700.0f;
            }
            else{
                _last_time = start_time;
            }            
            Adafruit_NeoPixel::setBrightness((uint8_t)(rate * 100));
            for(int i=0; i<_led_num; i++)
            {
                Adafruit_NeoPixel::setPixelColor(i, 
                    Adafruit_NeoPixel::Color(30, 30, 230));
            }
            break;
        case IllumiPattern::BOOTING:
            Adafruit_NeoPixel::setBrightness(80);
            if(dt < 800)
            {                
                cursor = int((float)dt / 800.0f * _led_num);
                if(cursor == 0){ cursor = 1; }                
                Adafruit_NeoPixel::setBrightness(30);
                for(int i=0; i<_led_num; i++){
                    if(i > cursor)
                    {                        
                        Adafruit_NeoPixel::setPixelColor(i, 
                            Adafruit_NeoPixel::Color(0, 0, 0));
                    }
                    else
                    {
                        rate = (float)i/(float)cursor;                        
                        Adafruit_NeoPixel::setPixelColor(i, 
                            Adafruit_NeoPixel::Color(
                                (uint8_t)(50*rate), (uint8_t)(255*rate), (uint8_t)(50*rate)));
                    }
                }            
            }
            else if(dt < 1600)
            {
                cursor = _led_num - int((dt - 800) / 800.0f * _led_num);
                if(cursor == 0){ cursor = 1; }
                Adafruit_NeoPixel::setBrightness(30);        
                for(int i=0; i<_led_num; i++){
                    if(i < cursor)
                    {                        
                        Adafruit_NeoPixel::setPixelColor(i, 
                            Adafruit_NeoPixel::Color(0, 0, 0));
                    }
                    else
                    {
                        rate = (float)(_led_num - i)/(float)(_led_num - cursor);
                        Adafruit_NeoPixel::setPixelColor(i, 
                            Adafruit_NeoPixel::Color(
                                (uint8_t)(50*rate), (uint8_t)(255*rate), (uint8_t)(50*rate)));
                    }
                }            
            }
            else
            {
                _last_time = start_time;
                _count++;

                if(_count >= 3)
                {
                    _pattern = IllumiPattern::IDLING;
                }
            }
            break;
        case IllumiPattern::IDLING:            
            Adafruit_NeoPixel::setBrightness(30);
            for(int i=0; i<_led_num; i++)
            {
                Adafruit_NeoPixel::setPixelColor(i, 
                    Adafruit_NeoPixel::Color(50, 255, 50));
            }
            break;
        case IllumiPattern::DAMAGING:
            Adafruit_NeoPixel::setBrightness(100);
            if(dt < 50)
            {
                for(int i=0; i<_led_num; i++)
                {
                        Adafruit_NeoPixel::setPixelColor(i, 
                            Adafruit_NeoPixel::Color(220, 20, 60));
                }
            }
            else if(dt < 100)
            {
                for(int i=0; i<_led_num; i++)
                {

                    Adafruit_NeoPixel::setPixelColor(i, 
                        Adafruit_NeoPixel::Color(0, 0, 0));                    
                }
            }
            else{
                _last_time = start_time;
                _count++;

                if(_count > 5)
                {
                    _pattern = IllumiPattern::IDLING;
                }
            }
            break;
        case IllumiPattern::FIRING:
            Adafruit_NeoPixel::clear();            
            if(dt < 200)
            {
                cursor = int((float)dt / 200.0f * _led_num);
                if(cursor == 0){ cursor = 1; }                
                Adafruit_NeoPixel::setBrightness(100);
                for(int i=0; i<_led_num; i++){
                    if(i > cursor)
                    {                        
                        Adafruit_NeoPixel::setPixelColor(i, 
                            Adafruit_NeoPixel::Color(0, 0, 0));
                    }
                    else
                    {
                        rate = (float)i/(float)cursor;                        
                        Adafruit_NeoPixel::setPixelColor(i, 
                            Adafruit_NeoPixel::Color((uint8_t)(255*rate), (uint8_t)(69*rate), 0));
                    }
                }            
            }
            else
            {
                _last_time = start_time;
                _pattern = IllumiPattern::IDLING;
            }
            break;
        case IllumiPattern::DOWN:
            Adafruit_NeoPixel::clear();
            Adafruit_NeoPixel::setBrightness(30);
            for(int i=0; i<_led_num; i++){
                Adafruit_NeoPixel::setPixelColor(i, 
                    Adafruit_NeoPixel::Color(230, 20, 20));
            }
            break;
    }

    Adafruit_NeoPixel::show();
}


void IllumiLED::switchLightingPattern(IllumiPattern pattern)
{
    _pattern = pattern;
    _last_time = millis();
    _count = 0;
}

