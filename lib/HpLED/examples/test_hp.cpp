#include <Arduino.h>

#include <HpLED.h>


HpLED hp(4, 26, NEO_GRB + NEO_KHZ800);


void setup()
{
    Serial.begin(115200);

    hp.begin();
}


void loop()
{
    hp.setHP(100);
    delay(1000);
    hp.setHP(75);
    delay(1000);
    hp.setHP(50);
    delay(1000);
    hp.setHP(25);
    delay(1000);
    hp.setHP(0);
    delay(1000);
}