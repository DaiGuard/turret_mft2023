#include <BluetoothPS4.h>
#include <SerialPS4.h>
#include <IllumiLED.h>
#include <HpLED.h>
#include <Speaker.h>
#include <FireControl.h>
#include <RobotSerial.h>

#include "macro.h"
#include "tasks.h"
#include "state_manager.h"


void mainLoop(void *args)
{
    // 送信キューを引数から取得する
    QueueHandle_t* paramsQueue = (QueueHandle_t*)args;
    BaseType_t result;

    // ロボットHP
    int robotHP = 100;

    // PS4コントローラ
    BluetoothPS4 bluetoothPS4;
    SerialPS4 serialPS4;
    HID * ps4;

    // 銃火器
    FireControl fire_control(21, 25, 1);

    // ロボット動作指示
    RobotSerial robotSerial;

    // デバック用シリアルの初期化
    Serial.begin(115200);

    // PS4コントローラ接続開始
    SUCCESS_CHK(bluetoothPS4.begin(), "can not begin bluetooth PS4");
    SUCCESS_CHK(serialPS4.begin(&Serial), "can not begin serial PS4");

    // DRV8834関連設定
    // DRV8835 Mode pin
    pinMode(23, OUTPUT);
    digitalWrite(23, HIGH);

    // DRV8835 A phase pin
    pinMode(22, OUTPUT);
    pinMode(22, LOW);

    // DRV8835 B enable pin
    pinMode(19, OUTPUT);
    pinMode(19, LOW);

    // 銃火器管制開始
    SUCCESS_CHK(fire_control.begin(), "can not begin fire control");

    // ロボット通信用シリアルの初期化
    Serial2.begin(115200);
    SUCCESS_CHK(robotSerial.begin(&Serial2), "can not start robot serial");

    TurretState currentState = TurretState::WAIT_CONNECT;
    TurretState prevState = TurretState::WAIT_CONNECT;

    while(1)
    {
        // ローカル変数
        int damege = 0;
        BaseType_t result;
        uint32_t soundType;
        uint32_t illumiType;

        // イベントリスト
        uint32_t events = (uint32_t)TurretEvent::EVENT_NONE;

        // コントローラ接続状態の確認
        bool bluetoothConnected = false;
        bool serialConnected = false;
        bluetoothConnected = bluetoothPS4.isConnected();    
        // serialConnected = serialPS4.isConnected();

        // 目標速度
        float vec[3] = {0.0f, 0.0f, 0.0f};  // vx, vy, rz
        bool fireSignal = false;
        bool resetSignal = false;

        // 各状態の処理
        switch((TurretState)((uint32_t)currentState & 0xf0))
        {
            case TurretState::WAIT_CONNECT:            
                if(bluetoothConnected || serialConnected)
                {
                    // イベント登録
                    events |= (uint32_t)TurretEvent::CONNECTED;

                    // 接続されたコントローラを使用
                    if(bluetoothConnected)
                    {
                        ps4 = &bluetoothPS4;
                    }
                    else if(serialConnected)
                    {
                        ps4 = &serialPS4;
                    }
                }
                break;
            case TurretState::INOPERATION:
                if(ps4->isConnected())
                {
                    //
                    // 動作中内のサブ状態処理が記載してない
                    //
                    // 現在HPをCore0へ送信する
                    result = xQueueSend(paramsQueue[0], &robotHP, 10);

                    // コントローラの値を取得
                    ps4->read();

                    // リセット指示
                    resetSignal = ps4->button.triangle > 0 ? true: false;
                    // 発砲指示
                    fireSignal = ps4->button.cross > 0 ? true: false;

                    vec[0] = ps4->analog.stick.ly / 127.0f;
                    vec[1] = ps4->analog.stick.lx / 127.0f;
                    vec[2] = ps4->analog.stick.rx / 127.0f;

                    
                    // 各動作指示                
                    switch(currentState)
                    {
                        case TurretState::INOPERATION_BOOTING:
                            // illumiType = (uint32_t)IllumiPattern::BOOTING;
                            // soundType = (uint32_t)SoundPattern::BOOT;
                            // result = xQueueSend(paramsQueue[1], &illumiType, 10);
                            // result = xQueueSend(paramsQueue[2], &soundType, 10);
                            break;
                        case TurretState::INOPERATION_RESET:
                            robotHP = 100;                        
                            break;
                        case TurretState::INOPERATION_ACTIVE:
                            // 発砲指示
                            if(fireSignal)
                            {
                                fire_control.fire();

                                // // 発砲指示をCore0に送る
                                // soundType = (uint32_t)SoundPattern::FIRE;                
                                // result = xQueueSend(paramsQueue[2], &soundType, 10);

                                // illumiType = (uint32_t)IllumiPattern::FIRING;
                                // result = xQueueSend(paramsQueue[1], &illumiType, 10);                            
                            }

                            // ダメージ確認
                            damege = fire_control.damage();                        
                            if(damege > 0)
                            {
                                robotHP = (robotHP - damege) >= 0 ? robotHP - damege : 0;
                            }

                            // ダウン判定
                            if(robotHP <= 0)
                            {
                                events |= (uint32_t)TurretEvent::ROBOT_DOWN;
                            }
                            // リセット指示
                            else if(resetSignal)
                            {
                                events |= (uint32_t)TurretEvent::ROBOT_RESET;
                            } 
                            break;
                        case TurretState::INOPERATION_DOWN:
                            if(resetSignal)
                            {
                                events |= (uint32_t)TurretEvent::ROBOT_RESET;
                            } 
                            break;
                    }
                }
                else
                {
                    // イベント登録
                    events |= (uint32_t)TurretEvent::DISCONNECTED;
                }
                break;
        }

        // 前回状態の保存
        prevState = currentState;

        // 状態の更新
        currentState = updateSate(currentState, events);  
    }
}


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
        result = xQueueReceive(paramsQueue[0], &power, 1);
        if(result == pdPASS){
            hp.setHP(power);
        }

        // Core1からイルミネーションデータのキューを確認する
        result = xQueueReceive(paramsQueue[1], &illumiType, 1);
        if(result == pdPASS){            
            if(illumiType > 0u)
            {
                illumi.switchLightingPattern((IllumiPattern)illumiType);
            }
        }

        illumi.update();
        
        // delay(30);
    }
}


// void displaySound(void *args)
// {
//     // 受信キューを引数から取得する
//     QueueHandle_t* paramsQueue = (QueueHandle_t*)args;
//     BaseType_t result;

//     // 各ドライバクラスを初期化
//     Speaker speaker(18, 1);

//     SUCCESS_CHK(speaker.begin(), "can not start speaker");

//     uint32_t soundType = (uint32_t)SoundPattern::NONE;

//     while(1){
//         // Core1からのサウンドデータのキューを確認する
//         result = xQueueReceive(paramsQueue[2], &soundType, 1);
//         if(result == pdPASS){
//             if(soundType > 0u)
//             {                
//                 speaker.playSound((SoundPattern)soundType);
//                 soundType = (uint32_t)SoundPattern::NONE;
//             }
//         }

//         // delay(30);
//     }
// }