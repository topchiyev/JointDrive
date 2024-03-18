#include <Arduino.h>
#include <Servo.h>
#include "ServoArmController.h"

const int armServoPin = PA8;
const int readyPulse = MIN_PULSE_WIDTH + (MAX_PULSE_WIDTH - MIN_PULSE_WIDTH) / 180 * 45;
const int freePulse = MAX_PULSE_WIDTH;

Servo armServo;

void ServoArmController::Begin()
{
    armServo.attach(armServoPin, freePulse);
    //this->SetReady();
    this->SetFree();
}

bool ServoArmController::IsReady()
{
    return this->isReady;
}

void ServoArmController::SetReady()
{
    armServo.write(readyPulse);
    this->isReady = true;
    delay(1000);
}

void ServoArmController::SetFree()
{
    armServo.write(freePulse);
    this->isReady = false;
    delay(1000);
}