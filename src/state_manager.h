#ifndef __STATE_MANAGER_H__
#define __STATE_MANAGER_H__


#include <stdint.h>


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


TurretState updateSate(TurretState state, uint32_t events);


#endif