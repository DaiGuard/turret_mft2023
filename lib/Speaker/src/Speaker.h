#ifndef __SPEAKER_H__
#define __SPEAKER_H__


enum class SoundPattern
{
    NONE = 0x00,
    BOOT = 0x01,
    FIRE = 0x02,
    DOWN = 0x03,
    MOVE = 0x04
};

class Speaker
{
    public:
        Speaker(int p, int ch);
        ~Speaker(){}

        bool begin();
        void playSound(SoundPattern sound);

    private:
        int _pin;
        int _ch;
};

#endif