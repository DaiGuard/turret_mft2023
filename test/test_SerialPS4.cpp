#include <Arduino.h>
#include <unity.h>
#include "SerialPS4.h"

HID *ps4 = new SerialPS4();

void setup()
{
    delay(2000); 
    UNITY_BEGIN();
    // シリアルポート初期化
    Serial.begin(115200);
    ps4->begin();

}


void loop() {
    if(ps4->isConnected())
    {
        ps4->read();
        if(ps4->button.down==0)
        {
            UNITY_END();
        }
    }
    
    // 待ち時間
    delay(30);
}