#include <Arduino.h>

#include <BluetoothPS4.h>
#include <SerialPS4.h>
#include <IllumiLED.h>
#include <HpLED.h>
#include <Speaker.h>
#include <FireControl.h>
#include <RobotSerial.h>

#include "macro.h"
#include "core0.h"


// マルチスレッドハンドラ
TaskHandle_t threadHandle[2];
QueueHandle_t paramsQueue[3];

// ロボットHP
int robotHP = 100;

// PS4コントローラ
BluetoothPS4 bluetoothPS4;
SerialPS4 serialPS4;
HID * ps4;

// LED関連
IllumiLED illumi(11, 27, NEO_GRB + NEO_KHZ800);
HpLED hp(4, 26, NEO_GRB + NEO_KHZ800);

// 銃火器
FireControl fire_control(21, 25, 1);

// ロボット動作指示
RobotSerial robotSerial;


enum class TurretState
{
    WAIT_CONNECT        = 0x00, // 接続待機中
    INOPERATION         = 0x10, // 動作中
    INOPERATION_RESET   = INOPERATION | 0x01,  // リセット中
    INOPERATION_BOOTING = INOPERATION | 0x02,  // 起動中
    INOPERATION_ACTIVE  = INOPERATION | 0x03,  // 活動中
    INOPERATION_DOWN    = INOPERATION | 0x04,  // ダウン中
};
enum class TurretEvent
{
    EVENT_NONE      = 0x00,
    CONNECTED       = 0x01,
    DISCONNECTED    = 0x02,
    ROBOT_DOWN      = 0x04,
    ROBOT_RESET     = 0x08,
};
TurretState currentState = TurretState::WAIT_CONNECT;
TurretState prevState = TurretState::WAIT_CONNECT;


TurretState updateSate(TurretState state, uint32_t events)
{
    TurretState new_state = state;

    switch((TurretState)((uint32_t)state & 0xf0))
    {
        case TurretState::WAIT_CONNECT:

            if((events & (uint32_t)TurretEvent::CONNECTED) > 0)
            {
                new_state = TurretState::INOPERATION_BOOTING;
            }

            break;
        case TurretState::INOPERATION:

            if((events & (uint32_t)TurretEvent::DISCONNECTED) > 0)
            {
                new_state = TurretState::WAIT_CONNECT;
            }            

            switch(state)
            {
                case TurretState::INOPERATION_RESET:
                    new_state = TurretState::INOPERATION_ACTIVE;
                    break;
                case TurretState::INOPERATION_BOOTING:
                    new_state = TurretState::INOPERATION_RESET;
                    break;
                case TurretState::INOPERATION_ACTIVE:
                    if((events & (uint32_t)TurretEvent::ROBOT_DOWN) > 0)
                    {
                        new_state = TurretState::INOPERATION_DOWN;
                    }     
                    else if((events & (uint32_t)TurretEvent::ROBOT_RESET) > 0)
                    {
                        new_state = TurretState::INOPERATION_RESET;
                    }     
                    break;
                case TurretState::INOPERATION_DOWN:
                    if((events & (uint32_t)TurretEvent::ROBOT_RESET) > 0)
                    {
                        new_state = TurretState::INOPERATION_RESET;
                    }     
                    break;
            }

            break;
    }
    return new_state;
}


void setup()
{
    // デバック用シリアルの初期化
    Serial.begin(115200);

    // PS4コントローラ接続開始
    SUCCESS_CHK(bluetoothPS4.begin(), "can not begin bluetooth PS4");
    SUCCESS_CHK(serialPS4.begin(&Serial), "can not begin serial PS4");

    // LED関連の初期化
    SUCCESS_CHK(illumi.begin(), "can not begin illumination LED");
    SUCCESS_CHK(hp.begin(), "can not begin hp LED");

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

    // Core0用スレッドを実行開始
    paramsQueue[0] = xQueueCreate(1, sizeof(int));
    SUCCESS_CHK(paramsQueue[0]!=NULL, "can not allocate hp queue");
    paramsQueue[1] = xQueueCreate(1, sizeof(uint32_t));    
    SUCCESS_CHK(paramsQueue[1]!=NULL, "can not allocate illumination queue");
    paramsQueue[2] = xQueueCreate(1, sizeof(uint32_t));
    SUCCESS_CHK(paramsQueue[2]!=NULL, "can not allocate sound queue");    
    xTaskCreatePinnedToCore(displayLED, "displayLED", 4096, &paramsQueue[0], 3, &threadHandle[0], 0);
    xTaskCreatePinnedToCore(displaySound, "displaySound", 4096 , &paramsQueue[0], 3, &threadHandle[1], 1);
}


void loop()
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
                        illumiType = (uint32_t)IllumiPattern::BOOTING;
                        soundType = (uint32_t)SoundPattern::BOOT;
                        result = xQueueSend(paramsQueue[1], &illumiType, 10);
                        result = xQueueSend(paramsQueue[2], &soundType, 10);
                        break;
                    case TurretState::INOPERATION_RESET:
                        robotHP = 100;                        
                        break;
                    case TurretState::INOPERATION_ACTIVE:
                        // 発砲指示
                        if(fireSignal)
                        {
                            fire_control.fire();

                            // 発砲指示をCore0に送る
                            soundType = (uint32_t)SoundPattern::FIRE;                
                            result = xQueueSend(paramsQueue[2], &soundType, 10);

                            illumiType = (uint32_t)IllumiPattern::FIRING;
                            result = xQueueSend(paramsQueue[1], &illumiType, 10);                            
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

    // Serial.print((uint32_t)currentState, HEX);
    // Serial.print(" ");
    // Serial.print((uint32_t)events, HEX);
    // Serial.println("");

    // 状態の更新
    currentState = updateSate(currentState, events);

    delay(30);
}