#include <Arduino.h>

#include <Speaker.h>


Speaker speaker(18, 1);


void setup()
{
    Serial.begin(115200);

    pinMode(23, OUTPUT);
    digitalWrite(23, HIGH);

    pinMode(19, OUTPUT);
    pinMode(19, LOW);

    speaker.begin();
}


void loop()
{
    speaker.playSound(SoundPattern::BOOT);
    delay(1000);
    speaker.playSound(SoundPattern::FIRE);
    delay(1000);
    speaker.playSound(SoundPattern::DOWN);
    delay(1000);
    speaker.playSound(SoundPattern::MOVE);
    delay(1000);
}