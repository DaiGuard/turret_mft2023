#include <Arduino.h>

#include "SerialPS4.h"


bool SerialPS4::begin()
{
  _serial = &Serial;
  
  return true;
}

bool SerialPS4::begin(HardwareSerial *serial)
{
  _serial = serial;

  return true;
}

void SerialPS4::read()
{
  if(Serial.available())
  {
    //文字列読込
    String data = _serial->readStringUntil('\n');//1,1,....,100.0(ボタン割り当て分の数値(,区切り))


    //文字列分解
    String split_data[18];//ボタン毎に分解して格納する
    int counter=0;
    for(; counter < 18; counter++)
    {
        int index = data.indexOf(',');//18個目はないがデータ数チェックのため、終了条件は18
        if(index < 0){break;}
        split_data[counter] = data.substring(0,index);
        data = data.substring(index+1, data.length());
    }
    split_data[17] = data;

    if(counter!=17){ return; }//文字数が不足する場合は無視

    //ボタン割り当て
    try
    {
        button.up = (uint8_t)atoi(split_data[0].c_str());
        button.right = (uint8_t)atoi(split_data[1].c_str());
        button.down = (uint8_t)atoi(split_data[2].c_str());
        button.left = (uint8_t)atoi(split_data[3].c_str());

        button.square = (uint8_t)atoi(split_data[4].c_str());
        button.cross = (uint8_t)atoi(split_data[5].c_str());
        button.circle = (uint8_t)atoi(split_data[6].c_str());
        button.triangle = (uint8_t)atoi(split_data[7].c_str());

        button.l1 = (uint8_t)atoi(split_data[8].c_str());
        button.r1 = (uint8_t)atoi(split_data[9].c_str());
        button.l2 = (uint8_t)atoi(split_data[10].c_str());
        button.r2 = (uint8_t)atoi(split_data[11].c_str());

        analog.button.l2 = atof(split_data[12].c_str());
        analog.button.r2 = atof(split_data[13].c_str());

        analog.stick.lx = atof(split_data[14].c_str());
        analog.stick.ly = atof(split_data[15].c_str());
        analog.stick.rx = atof(split_data[16].c_str());
        analog.stick.ry = atof(split_data[17].c_str());
    }
    catch(...)
    {
    }
    
  }
  return;
}
