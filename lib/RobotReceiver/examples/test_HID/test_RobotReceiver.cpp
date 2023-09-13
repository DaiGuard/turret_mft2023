#include <Arduino.h>

#include "RobotReceiver.h"

RobotReceiver rr;

void setup()
{
    // シリアルポート初期化
    Serial.begin(115200);

}


void loop() 
{
    if(rr.update())
    {
        float turn = rr.commands.turn;
    }
    
    // 待ち時間
    delay(30);
}