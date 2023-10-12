#include <Arduino.h>

#include "SerialPS4.h"

HID *ps4 = new SerialPS4();

void setup()
{
    // シリアルポート初期化
    Serial.begin(115200);
    ps4->begin();

}


void loop() {
    if(ps4->isConnected())
    {
        ps4->read();
    }
    
    // 待ち時間
    delay(30);
}