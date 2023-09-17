#include <Speaker.h>
#include <IllumiLED.h>
#include <HpLED.h>

#include "macro.h"
#include "core0.h"

void displayLED(void *args)
{
    // 受信キューを引数から取得する
    QueueHandle_t* paramsQueue = (QueueHandle_t*)args;
    BaseType_t result;

    // 各ドライバクラスを初期化
    IllumiLED illumi(11, 27, NEO_GRB + NEO_KHZ800);
    HpLED hp(4, 26, NEO_GRB + NEO_KHZ800);

    SUCCESS_CHK(illumi.begin(), "can not start illumination LED");
    SUCCESS_CHK(hp.begin(), "can not start hitpoint LED");

    int power = 100;    
    uint32_t illumiType = (uint32_t)IllumiPattern::NONE;

    while(1)
    {        
        // Core1からのHPデータのキューを確認する
        result = xQueueReceive(paramsQueue[0], &power, 10);
        if(result == pdPASS){
            hp.setHP(power);
        }

        // Core1からイルミネーションデータのキューを確認する
        result = xQueueReceive(paramsQueue[1], &illumiType, 10);
        if(result == pdPASS){
            Serial.println(illumiType);
            if(illumiType > 0u)
            {
                illumi.switchLightingPattern((IllumiPattern)illumiType);
            }
        }

        illumi.update();
        
        delay(10);
    }
}


void displaySound(void *args)
{
    // 受信キューを引数から取得する
    QueueHandle_t* paramsQueue = (QueueHandle_t*)args;
    BaseType_t result;

    // 各ドライバクラスを初期化
    Speaker speaker(18, 1);

    SUCCESS_CHK(speaker.begin(), "can not start speaker");

    uint32_t soundType = (uint32_t)SoundPattern::NONE;

    while(1){
        // Core1からのサウンドデータのキューを確認する
        result = xQueueReceive(paramsQueue[2], &soundType, 10);
        if(result == pdPASS){
            if(soundType > 0u)
            {                
                speaker.playSound((SoundPattern)soundType);
                soundType = (uint32_t)SoundPattern::NONE;
            }
        }

        delay(10);
    }
}