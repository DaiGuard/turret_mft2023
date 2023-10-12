#include <Arduino.h>
#include <SoftwareSerial.h>

#include "RobotReceiver.h"

#ifdef ESP32
EspSoftwareSerial::UART mySerial;
#else
SoftwareSerial mySerial(12, 11);
#endif

RobotReceiver rr(mySerial);


void setup()
{
    // シリアルポート初期化
    Serial.begin(115200);
#ifdef ESP32
    mySerial.begin(115200, EspSoftwareSerial::SWSERIAL_8N1, 12, 11);
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

      Serial.print("fb: "); Serial.print(rr.commands.fb); Serial.print(" ");
      Serial.print("lr: "); Serial.print(rr.commands.lr); Serial.print(" ");
      Serial.print("tr: "); Serial.print(rr.commands.turn); Serial.print(" ");
      Serial.print("up: "); Serial.print(rr.commands.up); Serial.print(" ");
      Serial.print("ri: "); Serial.print(rr.commands.right); Serial.print(" ");
      Serial.print("do: "); Serial.print(rr.commands.down); Serial.print(" ");
      Serial.print("le: "); Serial.print(rr.commands.left); Serial.print(" ");
      Serial.print("sq: "); Serial.print(rr.commands.square); Serial.print(" ");
      Serial.print("cr: "); Serial.print(rr.commands.cross); Serial.print(" ");
      Serial.print("ci: "); Serial.print(rr.commands.circle); Serial.print(" ");
      Serial.print("tr: "); Serial.print(rr.commands.triangle); Serial.println(" ");
    }
    
    // 待ち時間
    delay(30);
}
