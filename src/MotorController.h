#ifndef MOTOR_CONTROLLER
#define MOTOR_CONTROLLER

#include <stdint.h>
#include "Direction.h"

class MotorControllerDelegate
{
    public:
        virtual void OnMotorControllerStep(uint16_t portIndex, uint32_t distance) { }
        virtual void OnMotorControllerFinish(uint16_t portIndex) { }
};

class MotorController
{
    public:
        MotorController();
        void MoveFilament(
            uint16_t portIndex, uint16_t speed, Direction direction, uint16_t distance,
            MotorControllerDelegate * delegate);
        void Cancel();
        void Update(uint32_t time);
        
    private:
        bool isIdle = true;
        uint16_t portIndex = 0;
        uint16_t speed = 0;
        Direction direction = D_FORWARD;
        uint16_t distance = 0;
        uint16_t pastDistance = 0;
        uint16_t frequency = 100;
        uint32_t movementTime = 0;
        MotorControllerDelegate * delegate = nullptr;
        void MakeMovement();
};

#endif
