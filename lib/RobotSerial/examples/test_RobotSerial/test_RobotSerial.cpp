#include <Arduino.h>

#include "RobotSerial.h"

RobotSerial rs;
control_cmd cmd;

void setup()
{
    // シリアルポート初期化
    Serial.begin(115200);

    cmd.val.direct[0]=1.0;
    cmd.val.direct[1]=2.0;
    cmd.val.direct[2]=3.0;
    for(int i=0;i<2;i++)
    {
      cmd.val.buttons[i]=0xff;
    }
    cmd.val.crc16 = 0;

}

void loop() 
{
    rs.control(cmd.val.direct,0);
    
    // 待ち時間
    delay(30);
}