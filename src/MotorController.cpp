#include "MotorController.h"

MotorController::MotorController(){ }

void MotorController::MoveFilament(
    uint16_t portIndex, uint16_t speed, Direction direction, uint16_t distance,
    MotorControllerDelegate * delegate)
{
    if (!this->isIdle)
        return;
        
    this->portIndex = portIndex;
    this->speed = speed;
    this->direction = direction;
    this->distance = distance;
    this->pastDistance = 0;
    this->delegate = delegate;

    this->isIdle = false;
}

void MotorController::Cancel()
{
    if (this->isIdle)
        return;

    this->isIdle = true;
}

void MotorController::Update(uint32_t time)
{
    if (this->isIdle)
        return;

    if (this->movementTime > time)
        return;

    this->MakeMovement();
    this->movementTime = time + this->frequency;
}

void MotorController::MakeMovement()
{
    int32_t multiplier = 1;
    if (this->direction == D_BACKWARD)
        multiplier = -1;
        
    int32_t value = this->speed * multiplier;
    
    this->pastDistance += this->speed;
    this->delegate->OnMotorControllerStep(this->portIndex, value);
        
    if (this->pastDistance >= this->distance)
    {
        this->isIdle = true;
        this->delegate->OnMotorControllerFinish(this->portIndex);
    }
}