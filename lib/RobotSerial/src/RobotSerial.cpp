#include "RobotSerial.h"
#include "CRC.h"

void RobotSerial::control(control_cmd cmd)
{
    //CRC16生成
    cmd.val.crc16 = 0;
    uint16_t crc = calcCRC16(cmd.binary, 14, 0x8005, 0x0000, 0x0000, true, true);
    cmd.val.crc16 = crc;

    //送信用データ(16バイト＝スティック値(4)x3方向 +  ボタン(2) + CRC16(2))
    Serial.write(cmd.binary, 16);

}