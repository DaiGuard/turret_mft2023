#include <PS4Controller.h>
#include "BluetoothPS4.h"


bool BluetoothPS4::begin()
{
    uint8_t bt_mac[6];
    char bt_mac_str[18];

    esp_read_mac(bt_mac, ESP_MAC_BT);
    sprintf(bt_mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
        bt_mac[0], bt_mac[1], bt_mac[2], bt_mac[3], bt_mac[4], bt_mac[5]);
    
    return PS4.begin(bt_mac_str);
}

bool BluetoothPS4::isConnected()
{
    return PS4.isConnected();
}

void BluetoothPS4::read()
{
    if(PS4.isConnected()){
        button.up = PS4.data.button.up;
        button.right = PS4.data.button.right;
        button.down = PS4.data.button.down;
        button.left = PS4.data.button.left;

        button.square = PS4.data.button.square;
        button.cross = PS4.data.button.cross;
        button.circle = PS4.data.button.circle;
        button.triangle = PS4.data.button.triangle;

        button.l1 = PS4.data.button.l1;
        button.r1 = PS4.data.button.r1;
        button.l2 = PS4.data.button.l2;
        button.r2 = PS4.data.button.r2;

        analog.stick.lx = PS4.data.analog.stick.lx;
        analog.stick.ly = PS4.data.analog.stick.ly;
        analog.stick.rx = PS4.data.analog.stick.rx;
        analog.stick.ry = PS4.data.analog.stick.ry;

        analog.button.l2 = PS4.data.analog.button.l2;
        analog.button.r2 = PS4.data.analog.button.r2;
    }
}