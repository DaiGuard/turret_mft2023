#include "RobotReceiver.h"
#include "CRC.h"

bool RobotReceiver::update()
{
    if (!_serial.available()) return false;

    //フォーマット
    //16バイト＝ヘッダー(1)+スティック値(4)x3方向 +  ボタン(2) + CRC16(2)+フッター(1)
    uint8_t buf[18];

    buf[0] = _serial.read();

    if(buf[0]!=SerialHeader) return false;


    size_t len;
    len = _serial.readBytesUntil(SerialEnd, buf+1, 17);

    if(len != 16) return false;

    if(calcCRC16(buf, 17, 0x8005, 0x0000, 0x0000, true, true) != 0){return false;}

   //値代入
   commands.fb = cmd2float(buf,1);
   commands.lr = cmd2float(buf,5);
   commands.turn = cmd2float(buf,9);
   commands.up    = buf[13] & 0x01 > 0;
   commands.right = buf[13] & 0x02 > 0;
   commands.down  = buf[13] & 0x04 > 0;
   commands.left  = buf[13] & 0x08 > 0;
   commands.square   = buf[13] & 0x10 > 0;
   commands.cross    = buf[13] & 0x20 > 0;
   commands.circle   = buf[13] & 0x40 > 0;
   commands.triangle = buf[13] & 0x80 > 0;
   commands.l1  = buf[14] & 0x01 > 0;
   commands.r1  = buf[14] & 0x02 > 0;
   commands.l2  = buf[14] & 0x04 > 0;
   commands.r2  = buf[14] & 0x08 > 0;

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