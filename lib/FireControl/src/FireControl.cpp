#include "FireControl.h"

#include <IRutils.h>


FireControl::FireControl(int send_pin, int recv_pin, uint8_t no)
    : IRsend(send_pin), IRrecv(recv_pin), _DATA_HIGH(1650), _DATA_LOW(550)
{
    _send_pin = send_pin;
    _recv_pin = recv_pin;
    _no = no;

    for(int i=0; i<8; i++)
    {
        if((no & 0x80) > 0)
        {
            _rawData[i*2+0+18] = _DATA_HIGH;
        }
        else
        {
            _rawData[i*2+0+18] = _DATA_LOW;
        }
        _rawData[i*2+1+18] = 650;
        no = no << 1;
    }

    _last_time = millis();
}


bool FireControl::begin()
{
    pinMode(_send_pin, OUTPUT);
    IRsend::begin();
    IRrecv::enableIRIn();

    return true;
}


bool FireControl::fire()
{
    uint64_t now = millis();

    if((now - _last_time) > 800)
    {
        // IRsend::sendRaw(_rawData, 67, 38);
        IRsend::sendRaw(_rawData, 66, 38);
        _last_time = now;

        return true;
    }

    return false;
}


int FireControl::damage()
{
    int data = 0;
    decode_results results;

    if(IRrecv::decode(&results))
    {
        IRrecv::resume();
        
        uint16_t value = 0x0000;
        uint32_t buf;

        int header_check = 0;
        int recv_size = 0;

        for(int i=0; i<results.rawlen; i++){
            buf = results.rawbuf[i] * kRawTick;

            if(buf > 4000){
                header_check++;
                continue;
            }

            if(header_check >= 2){                
                if((recv_size & 1) == 0)
                {
                    value = value << 1;
                    if(buf > 1100){
                        value |= 0x0001;
                    }
                }

                recv_size++;
                if(recv_size >= 32){
                    break;
                }
            }            
        }     

        uint8_t recv_no = (uint8_t)(value & 0x00ff);
        uint8_t key = (uint8_t)((value >> 8) & 0x00ff);
        
        if(_no != recv_no && key == 0xa5){
            data = 25;
        }
    }

    return data;
}