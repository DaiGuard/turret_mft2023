#include <Arduino.h>

#include "RobotSerial.h"

RobotSerial rs;
float direct[3]={1.0, 2.0, 3.0};
bool buttons[12]={true,true,true,true,true,true,true,true,true,true,true,true};

void setup()
{
    // シリアルポート初期化
    Serial.begin(115200);

}


void loop() 
{
    rs.control(direct,buttons);
    
    // 待ち時間
    delay(30);
}