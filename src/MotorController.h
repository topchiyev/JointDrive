#ifndef MOTOR_CONTROLLER
#define MOTOR_CONTROLLER

#include <Arduino.h>
#include <TMC2209.h>
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
        virtual void OnMotorControllerSwitchingFinish(SwitchMotorPosition position) { }
        virtual void OnMotorControllerStep(uint16_t portIndex, uint32_t distance) { }
        virtual void OnMotorControllerFinish(uint16_t portIndex) { }
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
        void MoveFilament(
            uint16_t portIndex, uint16_t speed, Direction direction, uint16_t distance);
        void Stop();
        void Update(uint32_t time);
        
    private:
        uint16_t speed = 0;
        Direction direction = D_FORWARD;
        uint16_t distance = 0;
        uint16_t pastDistance = 0;
        uint16_t frequency = 50;
        uint32_t movementTime = 0;
        MotorControllerDelegate * delegate = nullptr;
        void MakeMovement();
        void HomingUpdate(uint32_t time);
        void NormalUpdate(uint32_t time);
};

#endif
