#ifndef __SPEAKER_H__
#define __SPEAKER_H__


enum class SoundPattern
{
    BOOT,
    FIRE,
    DOWN,
    MOVE
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