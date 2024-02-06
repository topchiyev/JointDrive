#include "SwitchInputController.h"

#define SWITCH_INPUT_PIN PC15
#define SWITCH_PULSE_LEN 600L // milliseconds
#define SWITCH_PULSE_ON_LEN 400L // milliseconds
#define SWITCH_PULSE_OFF_LEN 200L // milliseconds
#define SWITCH_COMMAND_NONE 0
#define SWITCH_COMMAND_FEED 5 // pulses

int bbb = 0;

void SwitchInputController::Begin(SwitchInputControllerDelegate * delegate)
{
    this->delegate = delegate;
    pinMode(SWITCH_INPUT_PIN, INPUT_PULLUP);
}

void SwitchInputController::Update(uint32_t time)
{
    bool pressed = digitalRead(SWITCH_INPUT_PIN) == LOW;
    if (this->isPressed != pressed && time > this->switchChangedOn + 20)
    {
        this->isPressed = pressed;
        this->switchChangedOn = time;

        this->isPulse = false;
        this->pulseChangesOn = time + SWITCH_PULSE_ON_LEN;
    }

    if (this->pulseChangesOn != 0 && time > this->pulseChangesOn)
    {
        this->isPulse = !this->isPulse;
        if (this->isPulse)
            this->pulseChangesOn = time + (SWITCH_PULSE_ON_LEN);
        else
            this->pulseChangesOn = time + (SWITCH_PULSE_OFF_LEN);
    }

    if (this->isPressed)
    {
        if (this->command == SWITCH_COMMAND_NONE && time >= this->switchChangedOn + (SWITCH_COMMAND_FEED * SWITCH_PULSE_LEN))
        {
            this->command = SWITCH_COMMAND_FEED;
        }
        else if (this->command == SWITCH_COMMAND_FEED && time >= this->switchChangedOn + (SWITCH_COMMAND_FEED * SWITCH_PULSE_LEN) + SWITCH_PULSE_LEN)
        {
            long portTime = time - (this->switchChangedOn + (SWITCH_COMMAND_FEED * SWITCH_PULSE_LEN));
            long exPortTime = portTime;
            long dec = exPortTime % SWITCH_PULSE_LEN;
            long intPortTime = portTime - dec;
            long pulses = intPortTime / SWITCH_PULSE_LEN;
            this->portIndex = (uint16_t)pulses;
        }
    }
    else if (this->command != SWITCH_COMMAND_NONE)
    {
        if (this->command == SWITCH_COMMAND_FEED && this->portIndex > 0 && this->portIndex <= 5)
        {
            this->delegate->OnSwitchFilamentChageRequest(this->portIndex);
        }
        this->command = SWITCH_COMMAND_NONE;
        this->portIndex = 0;
        this->isPulse = false;
        this->pulseChangesOn = 0;
    }
}

bool SwitchInputController::IsPressed()
{
    return this->isPressed;
}

bool SwitchInputController::IsPulse()
{
    return isPulse;
}