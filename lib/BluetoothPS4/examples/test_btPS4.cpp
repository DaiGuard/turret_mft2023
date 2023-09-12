#include <Arduino.h>

#include "BluetoothPS4.h"


BluetoothPS4 btPS4;

void setup()
{
    Serial.begin(115200);

    uint8_t bt_mac[6];
    char bt_str[18];
    esp_read_mac(bt_mac, ESP_MAC_BT);
    
    Serial.print("Bluetooth MAC: ");    
    Serial.print(bt_mac[0], HEX); Serial.print(":");
    Serial.print(bt_mac[1], HEX); Serial.print(":");
    Serial.print(bt_mac[2], HEX); Serial.print(":");
    Serial.print(bt_mac[3], HEX); Serial.print(":");
    Serial.print(bt_mac[4], HEX); Serial.print(":");
    Serial.print(bt_mac[5], HEX); Serial.println();

    if(!btPS4.begin()){
        Serial.println("can not begin PS4 controller");
        while(1){
            delay(100);
        }
    }
}


void loop()
{
    if(btPS4.isConnected()){
        btPS4.read();

        Serial.print("lx, ly = ");
        Serial.print(btPS4.analog.stick.lx); Serial.print(", ");
        Serial.println(btPS4.analog.stick.ly);
    }

    delay(100);
}