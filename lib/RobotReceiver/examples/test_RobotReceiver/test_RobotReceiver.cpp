#include <Arduino.h>

#include "RobotReceiver.h"

RobotReceiver rr(Serial1);

void setup()
{
    // シリアルポート初期化
    Serial.begin(115200);
    Serial1.begin(115200,SERIAL_8N1, 22,23);

    //LED
    pinMode(19, OUTPUT);
    digitalWrite(19, LOW);
}


void loop() 
{
  
  
    if(rr.update())
    {
      if(rr.commands.l1)
      {
        digitalWrite(19, HIGH);
      }else
      {
        digitalWrite(19, LOW);        
      }
      Serial.print("turn:");
      Serial.println(rr.commands.turn);

    }
    
    // 待ち時間
    delay(30);
}