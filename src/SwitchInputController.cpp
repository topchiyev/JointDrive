#include "SwitchInputController.h"

#define SWITCH_INPUT_PIN PC15
#define SWITCH_PULSE_LEN 500L // milliseconds
#define SWITCH_COOMAND_NONE 0
#define SWITCH_COMMAND_FEED 10 // pulses

uint32_t pulseOffTime = 0;
bool isPulse = false;

void SwitchInputController::Begin(SwitchInputControllerDelegate * delegate)
{
    this->delegate = delegate;
    pinMode(SWITCH_INPUT_PIN, INPUT_PULLUP);
}

void SwitchInputController::Update(uint32_t time)
{
    if (isPulse && time >= pulseOffTime)
    {
        isPulse = false;
    }

    int val = digitalRead(SWITCH_INPUT_PIN);
    bool pressed = val == HIGH;

    if (pressed != this->isPressed && time > this->stateChangedOn + 20)
    {
        this->isPressed = pressed;
        this->stateChangedOn = time;
    }

    if (!this->isPressed && this->command != SWITCH_COOMAND_NONE)
    {
        if (this->command == SWITCH_COMMAND_FEED && this->portIndex > 0 && this->portIndex <= 5)
        {
            this->delegate->OnSwitchFilamentChageRequest(this->portIndex);
        }
        this->command = SWITCH_COOMAND_NONE;
        this->portIndex = 0;
    }
    else if (this->command == SWITCH_COOMAND_NONE && this->isPressed && time > this->stateChangedOn + SWITCH_COMMAND_FEED * SWITCH_PULSE_LEN)
    {
        this->command = SWITCH_COMMAND_FEED;
    }
    else if (this->command == SWITCH_COMMAND_FEED && this->isPressed && time > this->stateChangedOn + (SWITCH_COMMAND_FEED * SWITCH_PULSE_LEN) + SWITCH_PULSE_LEN)
    {
        long portTime = time - (this->stateChangedOn + (SWITCH_COMMAND_FEED * SWITCH_PULSE_LEN));
        long exPortTime = portTime;
        long dec = exPortTime % SWITCH_PULSE_LEN;
        long intPortTime = portTime - dec;
        long pulses = intPortTime / SWITCH_PULSE_LEN;
        this->portIndex = (uint32_t)pulses;
        isPulse = true;
        pulseOffTime = time + 300;
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