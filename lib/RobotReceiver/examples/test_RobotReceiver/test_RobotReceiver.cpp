#include <Arduino.h>
#include <SoftwareSerial.h>

#include "RobotReceiver.h"

#define ESP_MODE

#ifdef ESP_MODE
EspSoftwareSerial::UART mySerial;
#else
SoftwareSerial mySerial(12, 11);
#endif

RobotReceiver rr(mySerial);


void setup()
{
    // シリアルポート初期化
    Serial.begin(115200);
#ifdef ESP_MODE
    mySerial.begin(115200, EspSoftwareSerial::SWSERIAL_8N1, 22, 23);
#else
    mySerial.begin(115200);
#endif

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
