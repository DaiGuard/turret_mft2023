#ifndef __ILLUMI_LED_H__
#define __ILLUMI_LED_H__


enum IllumiPatter
{
    WAITCONNECT,// 接続待機
    BOOTING,    // 起動中
    IDLING,     // 待機中
    DAMAGING,   // ダメージ
    DOWN,       // 戦闘不能
    FIRING,     // 発射中
};


class IllumiLED
{
    public:
        IllumiLED(){}
        ~IllumiLED(){}

        void update();
        void switchLightingPattern(int pattern);

    private:

};

#endif