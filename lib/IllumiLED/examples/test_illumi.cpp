#include <Arduino.h>

#include <IllumiLED.h>


IllumiLED illumi(11, 27, NEO_GRB + NEO_KHZ800);

uint64_t last_time = 0lu;
int pattern = IllumiPatter::WAITCONNECT;

void setup()
{
    Serial.begin(115200);

    illumi.begin();

    last_time = millis();
}


void loop()
{
    uint64_t start_time = millis();
    uint64_t dt = start_time - last_time;

    if(dt > 5000){
        last_time = start_time;

        switch(pattern)
        {
            case IllumiPatter::WAITCONNECT:
                pattern = IllumiPatter::BOOTING;
                break;
            case IllumiPatter::BOOTING:
                pattern = IllumiPatter::IDLING;
                break;
            case IllumiPatter::IDLING:
                pattern = IllumiPatter::FIRING;
                break;
            case IllumiPatter::FIRING:
                pattern = IllumiPatter::DAMAGING;
                break;
            case IllumiPatter::DAMAGING:
                pattern = IllumiPatter::DOWN;
                break;
            case IllumiPatter::DOWN:
                pattern = IllumiPatter::WAITCONNECT;
                break;
        }
        
        illumi.switchLightingPattern(pattern);
    }

    Serial.print(dt); Serial.print("(ms): ");
    Serial.print(pattern);
    Serial.println();
        
    illumi.update();
    delay(10);
}