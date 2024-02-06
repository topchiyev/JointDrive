#include <Arduino.h>

#ifndef SWITCH_INPUT_CONTROLLER
#define SWITCH_INPUT_CONTROLLER 1

class SwitchInputControllerDelegate
{
    public:
        virtual void OnSwitchFilamentChageRequest(uint16_t portIndex) { }
};

class SwitchInputController
{
    public:
        void Begin(SwitchInputControllerDelegate * delgate);
        void Update(uint32_t time);
        bool IsPressed();
        bool IsPulse();
    
    private:
        SwitchInputControllerDelegate * delegate = nullptr;
        bool isPressed = false;
        uint32_t switchChangedOn = 0;
        uint32_t pulseChangesOn = 0;
        bool isPulse = false;
        uint8_t command = 0;
        uint16_t portIndex = 0;
};

#endif