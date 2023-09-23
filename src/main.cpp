#include <Arduino.h>

#include "macro.h"
// #include "tasks.h"
#include "state_manager.h"

#include <BluetoothPS4.h>
#include <SerialPS4.h>
#include <IllumiLED.h>
#include <HpLED.h>
#include <Speaker.h>
#include <FireControl.h>
#include <RobotSerial.h>


// マルチスレッドハンドラ
TaskHandle_t threadHandle[3];
QueueHandle_t paramsQueue[3];

// ロボットHP
int robotHP = 100;

// PS4コントローラ
BluetoothPS4 bluetoothPS4;
SerialPS4 serialPS4;
HID * ps4;

// 銃火器
// FireControl fire_control(21, 25, 1);
FireControl fire_control(21, 25, 2);

// LED表示
IllumiLED illumi(11, 27, NEO_GRB + NEO_KHZ800);
HpLED hp(4, 26, NEO_GRB + NEO_KHZ800);

// スピーカ
Speaker speaker(18, 0);

// ロボット動作指示
RobotSerial robotSerial;

// 現在状態
TurretState currentState = TurretState::WAIT_CONNECT;
TurretState prevState = TurretState::WAIT_CONNECT;

// サウンドパターン
SoundPattern soundTarget = SoundPattern::NONE;


void setup()
{
    // デバック用シリアルの初期化
    Serial.begin(115200);
    
    // PS4コントローラ接続開始
    SUCCESS_CHK(bluetoothPS4.begin(), "can not begin bluetooth PS4");
    SUCCESS_CHK(serialPS4.begin(&Serial), "can not begin serial PS4");

    // LED表示開始
    SUCCESS_CHK(illumi.begin(), "can not start illumination LED");
    SUCCESS_CHK(hp.begin(), "can not start hitpoint LED");

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

    // スピーカ開始
    SUCCESS_CHK(speaker.begin(), "can not start speaker");

    // ロボット通信用シリアルの初期化
    Serial2.begin(115200);
    SUCCESS_CHK(robotSerial.begin(&Serial2), "can not start robot serial");

    // Core0用スレッドを実行開始
    // disableCore0WDT();
    paramsQueue[0] = xQueueCreate(4, sizeof(int));
    SUCCESS_CHK(paramsQueue[0]!=NULL, "can not allocate hp queue");
    paramsQueue[1] = xQueueCreate(4, sizeof(uint32_t));    
    SUCCESS_CHK(paramsQueue[1]!=NULL, "can not allocate illumination queue");
    paramsQueue[2] = xQueueCreate(4, sizeof(uint32_t));
    SUCCESS_CHK(paramsQueue[2]!=NULL, "can not allocate sound queue");    
    // xTaskCreatePinnedToCore(displayLED, "displayLED", 4096, &paramsQueue[0], 3, &threadHandle[0], 0);
    // xTaskCreatePinnedToCore(displaySound, "displaySound", 8192 , &paramsQueue[0], 3, &threadHandle[1], 0);
    // xTaskCreatePinnedToCore(mainLoop, "mainLoop", 8192 , &paramsQueue[0], 3, &threadHandle[2], 0);
}


void loop()
{    
    // ローカル変数
    int damege = 0;
    BaseType_t result;
    uint32_t soundType;
    uint32_t illumiType;
    control_cmd command;

    // イベントリスト
    uint32_t events = (uint32_t)TurretEvent::EVENT_NONE;

    // コントローラ接続状態の確認
    bool bluetoothConnected = false;
    bool serialConnected = false;
    bluetoothConnected = bluetoothPS4.isConnected();    
    // serialConnected = serialPS4.isConnected();

    // 目標速度
    float vec[3] = {0.0f, 0.0f, 0.0f};  // vx, vy, rz
    uint16_t buttons;
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
                hp.setHP(robotHP);

                // コントローラの値を取得
                ps4->read();

                // リセット指示
                resetSignal = ps4->button.triangle > 0 ? true: false;
                // 発砲指示
                fireSignal = ps4->button.cross > 0 ? true: false;

                vec[0] = - ps4->analog.stick.ly / 127.0f;
                vec[1] = ps4->analog.stick.lx / 127.0f;
                vec[2] = ps4->analog.stick.rx / 127.0f;

                buttons = 0x0000;
                buttons |= ps4->button.up > 0       ? 0x0001 : 0x0000;
                buttons |= ps4->button.right > 0    ? 0x0002 : 0x0000;
                buttons |= ps4->button.down > 0     ? 0x0004 : 0x0000;
                buttons |= ps4->button.left > 0     ? 0x0008 : 0x0000;
                buttons |= ps4->button.square > 0   ? 0x0010 : 0x0000;
                buttons |= ps4->button.cross > 0    ? 0x0020 : 0x0000;
                buttons |= ps4->button.circle > 0   ? 0x0040 : 0x0000;
                buttons |= ps4->button.triangle > 0 ? 0x0080 : 0x0000;
                buttons |= ps4->button.l1 > 0       ? 0x0100 : 0x0000;
                buttons |= ps4->button.r1 > 0       ? 0x0200 : 0x0000;
                buttons |= ps4->button.l2 > 0       ? 0x0400 : 0x0000;
                buttons |= ps4->button.r2 > 0       ? 0x0800 : 0x0000;

                // 各動作指示                
                switch(currentState)
                {
                    case TurretState::INOPERATION_BOOTING:
                        illumi.switchLightingPattern(IllumiPattern::BOOTING);
                        speaker.playSound(SoundPattern::BOOT);
                        break;
                    case TurretState::INOPERATION_RESET:
                        robotHP = 100;                        
                        break;
                    case TurretState::INOPERATION_ACTIVE:
                        // ロボットへコマンド送信
                        robotSerial.control(vec, buttons);

                        // ダメージ確認
                        damege = fire_control.damage();                        
                        if(damege > 0)
                        {
                            illumi.switchLightingPattern(IllumiPattern::DAMAGING);                            
                            robotHP = (robotHP - damege) >= 0 ? robotHP - damege : 0;
                        }

                        // 発砲指示
                        if(fireSignal)
                        {
                            if(fire_control.fire())
                            {
                              // 発砲指示をCore0に送る
                              illumi.switchLightingPattern(IllumiPattern::FIRING);
                              speaker.playSound(SoundPattern::FIRE);
                            }
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
                        illumi.switchLightingPattern(IllumiPattern::DOWN);
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

    // 表示更新
    illumi.update();

    // 前回状態の保存
    prevState = currentState;

    // 状態の更新
    currentState = updateSate(currentState, events);

    delay(30);
}