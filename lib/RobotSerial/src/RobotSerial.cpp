#include "RobotSerial.h"
#include "CRC.h"


void RobotSerial::control(float* direct, bool* buttons)
{
    //配列サイズチェック
    if(sizeof(direct) / sizeof(float) != 3){return;}
    if(sizeof(buttons) / sizeof(bool) != 12){return;}

    //コマンド：14バイト＝スティック値(4)x3方向 +  ボタン(2)
    uint8_t data[14] = {0};

    //バイナリ変換
    for(int i=0; i<3; i++)
    {
        uint8_t* bytes = (uint8_t*)&direct[i]; // float値の先頭アドレスをuint8_tポインタにキャストする
        for(int j=0;j<4;j++)
        {
            data[i*4+j] = bytes[j];
        }
    }
    for(int i=0;i<8;i++)
    {
        data[12] += (uint8_t)buttons[i] << i;
    }
    for(int i=0;i<4;i++)
    {
        data[13] += (uint8_t)buttons[8+i] << i;
    }

    //CRC16生成
    uint16_t crc = calcCRC16(data, 14, 0x8005, 0x0000, 0x0000, true, true);
    uint8_t* crcs = (uint8_t*)&crc;

    //送信用データ生成(16バイト＝スティック値(4)x3方向 +  ボタン(2) + CRC16(2))
    uint8_t send_data[16] = {0};
    memcpy(send_data, data, sizeof(data));
    for(int i=0;i<2;i++)
    {
        send_data[14+i] = crcs[i];
    }

    //送信
    Serial.write(send_data, 16);

}

