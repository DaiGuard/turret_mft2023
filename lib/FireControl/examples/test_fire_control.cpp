#include <Arduino.h>

#include "FireControl.h"


FireControl fire_control(21, 25, 1);


void setup()
{
    Serial.begin(115200);

    // DRV8835 Mode pin
    pinMode(23, OUTPUT);
    digitalWrite(23, HIGH);

    // DRV8835 A phase pin
    pinMode(22, OUTPUT);
    pinMode(22, LOW);

    // DRV8835 A enable pin
    pinMode(21, OUTPUT);
    fire_control.begin();
}


void loop()
{
    int damage = fire_control.damage();
    if(damage > 0){
        Serial.print("recv damage: ");
        Serial.println(damage);
    }
    else{
        delay(1000);
        Serial.println("Fire");
        fire_control.fire();
    }

    delay(100);
}