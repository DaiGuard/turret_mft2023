#include "state_manager.h"


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
