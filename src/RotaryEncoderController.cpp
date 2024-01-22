#include "RotaryEncoderController.h"
#include <RotaryEncoder.h>

uint16_t pinSwitch = 0;

RotaryEncoderControllerDelegate * delegateObj = nullptr;
bool isSwitchPressed = false;
RotaryEncoder * encoder = nullptr;
long lastEncoderPos = 0;


void CheckRotaryPos();

void RotaryEncoderController::Begin(uint16_t clk, uint16_t dt, uint16_t sw, RotaryEncoderControllerDelegate * delegate)
{
    pinSwitch = sw;
    delegateObj = delegate;
    pinMode(pinSwitch, INPUT);
    encoder = new RotaryEncoder(clk, dt, RotaryEncoder::LatchMode::FOUR0);

    attachInterrupt(digitalPinToInterrupt(clk), CheckRotaryPos, CHANGE);
    attachInterrupt(digitalPinToInterrupt(dt), CheckRotaryPos, CHANGE);
}

void CheckRotaryPos()
{
    encoder->tick();
}

void RotaryEncoderController::Update(uint32_t time)
{
    bool isPressed = digitalRead(pinSwitch) == LOW;
    if (isPressed != isSwitchPressed)
    {
        isSwitchPressed = isPressed;
        if (isSwitchPressed)
        {
            delegateObj->OnRotaryEncoderPressed();
        }
    }

    long pos = encoder->getPosition();
    if (pos != lastEncoderPos)
    {
        RotaryEncoder::Direction dir = encoder->getDirection();
        if (dir == RotaryEncoder::Direction::CLOCKWISE)
        {
            delegateObj->OnRotaryEncoderDecreased();
        }
        else if (dir == RotaryEncoder::Direction::COUNTERCLOCKWISE)
        {
            delegateObj->OnRotaryEncoderIncreased();
        }
        lastEncoderPos = pos;
    }
}