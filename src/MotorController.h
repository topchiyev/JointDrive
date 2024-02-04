#ifndef MOTOR_CONTROLLER
#define MOTOR_CONTROLLER

#include <Arduino.h>
#include "Direction.h"
#include "SwitchMotorPosition.h"


class MotorControllerDelegate
{
    public:
        virtual void OnMotorControllerFinishedHoming() { }
        virtual void OnMotorControllerFinishedSwitching(SwitchMotorPosition position) { }
        virtual void OnMotorControllerMoved(uint16_t portIndex, uint32_t distanceGone, Direction direction) { }
        virtual void OnMotorControllerFinishedMoving(uint16_t portIndex) { }
};

class MotorController
{
    public:
        bool isIdle = true;
        uint16_t portIndex = 0;
        bool isHoming = false;
        uint32_t distance = 0;
        Direction direction = D_FORWARD;
        
        void Begin(MotorControllerDelegate * delegate);
        SwitchMotorPosition GetSwitchPosition();
        void Home();
        SwitchMotorPosition PortIndexToSwitchPosition(uint8_t portIndex);
        void SwitchMotorToPosition(SwitchMotorPosition position);
        void MoveFilament(uint16_t portIndex, Direction direction, uint32_t distance);
        void Stop();
        void Update(uint32_t time);
        uint32_t GetSgResult();
        
    private:
        MotorControllerDelegate * delegate = nullptr;
        void NormalUpdate(uint32_t time);
        void HomingUpdate(uint32_t time);
};

#endif
