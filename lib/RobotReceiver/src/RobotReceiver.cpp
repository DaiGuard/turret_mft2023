#include "RobotReceiver.h"
#include "CRC.h"

bool RobotReceiver::update()
{
    //フォーマット
    //18バイト＝ヘッダー(1) + スティック値(4)x3方向 +  ボタン(2) + CRC16(2) + フッター(1)

    uint8_t buf[128];
    size_t len;
    uint16_t recv_crc, crc;    

    while(_serial.available())
    {
        int header = _serial.read();
        if(header != SerialHeader)
        {
            Serial.println("non header");
            continue;
        }
        buf[0] = (uint8_t)header;

        len = _serial.readBytesUntil(SerialEnd, buf+1, 127);
        if(len != 16)
        {
            Serial.println("no data lenght");
            continue;
        }

        memcpy(&recv_crc, buf+15, sizeof(uint16_t));

        crc = calcCRC16(buf, 15, 0x8005, 0x0000, 0x0000, true, true);

        if(recv_crc != crc)
        {
            Serial.println("no crc");
            continue;
        }

        //値代入
        commands.fb = cmd2float(buf,1);
        commands.lr = cmd2float(buf,5);
        commands.turn = cmd2float(buf,9);
        commands.up    = (buf[13] & 0x01) > 0;
        commands.right = (buf[13] & 0x02) > 0;
        commands.down  = (buf[13] & 0x04) > 0;
        commands.left  = (buf[13] & 0x08) > 0;
        commands.square   = (buf[13] & 0x10) > 0;
        commands.cross    = (buf[13] & 0x20) > 0;
        commands.circle   = (buf[13] & 0x40) > 0;
        commands.triangle = (buf[13] & 0x80) > 0;
        commands.l1  = (buf[14] & 0x01) > 0;
        commands.r1  = (buf[14] & 0x02) > 0;
        commands.l2  = (buf[14] & 0x04) > 0;
        commands.r2  = (buf[14] & 0x08) > 0;

        return true;        
    }

    return false;

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