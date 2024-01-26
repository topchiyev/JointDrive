#ifndef MOTOR_CONTROLLER
#define MOTOR_CONTROLLER

#include <Arduino.h>
#include "Direction.h"

enum SwitchMotorPosition
{
    SMP_UNKNOWN = 0,
    SMP_RIGHT = 1,
    SMP_CENTER = 2,
    SMP_LEFT = 3,
};

class MotorControllerDelegate
{
    public:
        virtual void OnMotorControllerFinishedHoming() { }
        virtual void OnMotorControllerFinishedSwitching(SwitchMotorPosition position) { }
        virtual void OnMotorControllerMoved(uint16_t portIndex, uint32_t distanceLeft) { }
        virtual void OnMotorControllerFinishedMoving(uint16_t portIndex) { }
};

class MotorController
{
    public:
        bool isIdle = true;
        uint16_t portIndex = 0;
        
        void Begin(MotorControllerDelegate * delegate);
        SwitchMotorPosition GetSwitchPosition();
        void Home();
        void SwitchMotorToPosition(SwitchMotorPosition position);
        void MoveFilament(uint16_t portIndex, Direction direction, uint16_t distance);
        void Stop();
        void Update(uint32_t time);
        uint32_t GetSgResult();
        
    private:
        MotorControllerDelegate * delegate = nullptr;
        void MakeMovement();
        void NormalUpdate(uint32_t time);
        void HomingUpdate(uint32_t time);
};

#endif
