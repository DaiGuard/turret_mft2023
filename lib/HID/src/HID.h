#ifndef __HUMAN_INTERFACE_DEVICE_H__
#define __HUMAN_INTERFACE_DEVICE_H__


#include <stdint.h>


typedef struct {
    float lx;
    float ly;
    float rx;
    float ry;
} hid_analog_stick_t;

typedef struct {
    float l2;
    float r2;
} hid_analog_button_t;

typedef struct {
    uint8_t up       : 1;
    uint8_t right    : 1;
    uint8_t down     : 1;
    uint8_t left     : 1;

    uint8_t square   : 1;
    uint8_t cross    : 1;
    uint8_t circle   : 1;
    uint8_t triangle : 1;
    
    uint8_t l1       : 1;
    uint8_t r1       : 1;
    uint8_t l2       : 1;
    uint8_t r2       : 1;    
} hid_button_t;

typedef struct {
    hid_analog_button_t button; 
    hid_analog_stick_t stick;
} hid_analog_t;


class HID {
    public: 
        HID(){
            button.up = 0u;
            button.right = 0u;
            button.down = 0u;
            button.left = 0u;

            button.square = 0u;
            button.cross = 0u;
            button.circle = 0u;
            button.triangle = 0u;

            button.l1 = 0u;
            button.r1 = 0u;
            button.l2 = 0u;
            button.r2 = 0u;

            analog.button.l2 = 0.0f;
            analog.button.r2 = 0.0f;

            analog.stick.lx = 0.0f;
            analog.stick.ly = 0.0f;
            analog.stick.rx = 0.0f;
            analog.stick.ry = 0.0f;
        }

        virtual ~HID() = default;

        virtual bool isConnected() = 0;
        virtual bool begin() = 0;

        virtual void read() = 0;

        hid_button_t button;
        hid_analog_t analog;
};


#endif