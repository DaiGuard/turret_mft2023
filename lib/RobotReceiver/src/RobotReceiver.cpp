#include "RobotReceiver.h"
#include "CRC.h"

RobotReceiver::RobotReceiver()
{
}
bool RobotReceiver::update()
{
    if (!Serial.available()) return false;

    //フォーマット
    //16バイト＝スティック値(4)x3方向 +  ボタン(2) + CRC16(2)
    uint8_t buf[16];
    Serial.readBytes(buf, 16);

    //CRC16チェック
    if(calcCRC16(buf, 16, 0x8005, 0x0000, 0x0000, true, true) != 0){return false;}

    //値代入
    commands.fb = cmd2float(buf,0);
    commands.lr = cmd2float(buf,4);
    commands.turn = cmd2float(buf,8);
    commands.up    = buf[12] & 0x01 > 0;
    commands.right = buf[12] & 0x02 > 0;
    commands.down  = buf[12] & 0x04 > 0;
    commands.left  = buf[12] & 0x08 > 0;
    commands.square   = buf[12] & 0x10 > 0;
    commands.cross    = buf[12] & 0x20 > 0;
    commands.circle   = buf[12] & 0x40 > 0;
    commands.triangle = buf[12] & 0x80 > 0;
    commands.l1  = buf[13] & 0x01 > 0;
    commands.r1  = buf[13] & 0x02 > 0;
    commands.l2  = buf[13] & 0x04 > 0;
    commands.r2  = buf[13] & 0x08 > 0;

    return true;

}

float RobotReceiver::cmd2float(uint8_t *data, int index)
{
    uint8_t cmd[4];
    for(int i=0;i<4;i++)
    {
        cmd[i]=data[index+i];
    }
    return *(float*)cmd;
}
