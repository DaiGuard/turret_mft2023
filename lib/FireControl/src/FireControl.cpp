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
            _rawData[i*2+0+19] = _DATA_HIGH;
        }
        else
        {
            _rawData[i*2+0+19] = _DATA_LOW;
        }
        _rawData[i*2+1+19] = 650;
        no = no << 1;
    }
}


bool FireControl::begin()
{
    pinMode(_send_pin, OUTPUT);
    IRsend::begin();
    IRrecv::enableIRIn();

    return true;
}


void FireControl::fire()
{    
    IRsend::sendRaw(_rawData, 67, 38);
}


int FireControl::damage()
{
    int data = 0;
    decode_results results;

    if(IRrecv::decode(&results))
    {
        uint8_t recv_no = (results.value >> 16) & 0x00ff;
        IRrecv::resume();
        uint8_t key = (results.value >> 24) & 0x00ff;

        if(_no != recv_no && key == 0xa5){
            data = 25;
        }
    }

    return data;
}