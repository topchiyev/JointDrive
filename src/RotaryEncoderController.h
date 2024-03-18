#ifndef ROTARY_ENCODER_CONTROLLER_H
#define ROTARY_ENCODER_CONTROLLER_H

#include <Arduino.h>

class RotaryEncoderControllerDelegate
{
    public:
        virtual void OnRotaryEncoderIncreased() { }
        virtual void OnRotaryEncoderDecreased() { }
        virtual void OnRotaryEncoderPressed() { }
};

class RotaryEncoderController
{
    public:
        void Begin(uint16_t clk, uint16_t dt, uint16_t sw, RotaryEncoderControllerDelegate * delegate);
        void Update(uint32_t time);
};

#endif