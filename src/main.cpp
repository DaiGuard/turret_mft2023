#include <Arduino.h>

#include <BluetoothPS4.h>
#include <SerialPS4.h>
#include <IllumiLED.h>
#include <HpLED.h>
#include <Speaker.h>
#include <FireControl.h>


#define SUCCESS_CHK(f, msg) if(f){ \
                                while(1){ Serial.println(msg); } \
                            }

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


enum class TurretState
{
    WAIT_CONNECT        = 0x00, // 接続待機中
    INOPERATION         = 0x10, // 動作中
    INOPERATION_RESET   = INOPERATION | 0x01,  // リセット中
    INOPERATION_BOOTING = INOPERATION | 0x02,  // 起動中
    INOPERATION_FIRING  = INOPERATION | 0x03,  // 発砲中
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
    TurretState new_state;

    switch((TurretState)((uint32_t)currentState & 0xf0))
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

            //
            // TODO: 状態遷移は実装途中
            //
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
    SUCCESS_CHK(serialPS4.begin(), "can not begin serial PS4");

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

    //
    // TODO: サブタスクにLEDとスピーカを割り当てること
    //
}


void loop()
{
    // ローカル変数
    int damege = 0;

    // イベントリスト
    uint32_t events = (uint32_t)TurretEvent::EVENT_NONE;

    // コントローラ接続状態の確認
    bool bluetoothConnected = bluetoothPS4.isConnected();
    bool serialConnected = serialPS4.isConnected();

    // 目標速度
    float vec[3] = {0.0f, 0.0f, 0.0f};  // vx, vy, rz

    // 各状態の処理
    switch((TurretState)((uint32_t)currentState & 0xf0))
    {
        case TurretState::WAIT_CONNECT:            
            if(bluetoothConnected | serialConnected)
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

                // コントローラの値を取得
                ps4->read();

                // 攻撃要求確認
                if(ps4->button.circle > 0)
                {
                    // 発砲指示
                    fire_control.fire();
                }

                vec[0] = ps4->analog.stick.ly / 127.0f;
                vec[1] = ps4->analog.stick.lx / 127.0f;
                vec[2] = ps4->analog.stick.rx / 127.0f;
                //
                // TODO: ロボットへの動作指示を実施
                //

                // ダメージ確認
                damege = fire_control.damage();
                if(damege > 0)
                {
                    robotHP = (robotHP - damege) >= 0 ? robotHP - damege : 0;
                }

                if(robotHP <= 0)
                {
                    events |= (uint32_t)TurretEvent::ROBOT_DOWN;
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