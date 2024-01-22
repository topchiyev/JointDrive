#include "MotorController.h"
#include <TMC2209.h>

// 1 STEP = 1.8 DEGREE
// 1 STEP = 8 MICROSTEPS
// 1 MICROSTEP = 0,225 DEGREE
// 360 DEGREE = 200 STEPS
// 360 DEGREE = 1600 MICROSTEPS

const uint32_t MAXIMUM_DELAY = 1000;
const uint32_t SWITCH_RIGHT_STEP = 30000;
const uint32_t SWITCH_CENTER_STEP = 15000;
const uint32_t SWITCH_LEFT_STEP = 0;
const uint32_t SWITCH_HOMING_EXTRA_STEP = 3000;

struct Motor
{
    uint8_t enPin = 0;
    bool isSwitchMotor = false;
    SwitchMotorPosition switchPosition = SMP_UNKNOWN;
    int32_t velocity = 1000;
    int32_t stepsPerMm = 1;
    const uint8_t stallguardThreshold = 30;
    int64_t curStep = -1;
    uint64_t buffer = 0;
    TMC2209::SerialAddress driverAddress;
    TMC2209 driver;
    uint64_t directionChangedOn = 0;
    bool direction = true;
};

// Switch stallguard
const long BAUD_RATE = 9600l;
const uint8_t RX_PIN = PC11;
const uint8_t TX_PIN = PC10;
SoftwareSerial serial(RX_PIN, TX_PIN);

bool isHoming = false;


// X
Motor switchMotor;
// Y
Motor feedMotor1;
// Z
Motor feedMotor2;
// E
Motor feedMotor3;

Motor * allMotors[4];


void MotorController::Begin(MotorControllerDelegate * delegate)
{
    this->delegate = delegate;

    switchMotor.enPin = PB14;
    switchMotor.driverAddress = TMC2209::SerialAddress::SERIAL_ADDRESS_0;
    
    feedMotor1.enPin = PB11;
    feedMotor1.driverAddress = TMC2209::SerialAddress::SERIAL_ADDRESS_2;

    feedMotor2.enPin = PB1;
    feedMotor2.driverAddress = TMC2209::SerialAddress::SERIAL_ADDRESS_3;

    feedMotor3.enPin = PD1;
    feedMotor3.driverAddress = TMC2209::SerialAddress::SERIAL_ADDRESS_1;

    allMotors[0] = &switchMotor;
    allMotors[1] = &feedMotor1;
    allMotors[2] = &feedMotor2;
    allMotors[3] = &feedMotor3;

    for (uint8_t i = 0; i < 4; i++)
    {
        Motor * motor = allMotors[i];
        if (i == 0)
        {
            motor->isSwitchMotor = true;
        }

        motor->driver.setup(serial, BAUD_RATE, motor->driverAddress);
        motor->driver.setStallGuardThreshold(motor->stallguardThreshold);
        motor->driver.setMicrostepsPerStep(8);
        motor->driver.setRunCurrent(100);
        motor->driver.setHardwareEnablePin(switchMotor.enPin);
        motor->driver.enable();
    }
}

void MotorController::Home()
{
    if (!isIdle)
        return;

    isHoming = true;
    switchMotor.buffer = SWITCH_RIGHT_STEP + SWITCH_HOMING_EXTRA_STEP;
    switchMotor.driver.disableInverseMotorDirection();
    switchMotor.driver.moveAtVelocity(switchMotor.velocity);

    isIdle = false;
}

SwitchMotorPosition MotorController::GetSwitchPosition()
{
    return switchMotor.switchPosition;
}

void MotorController::SwitchMotorToPosition(SwitchMotorPosition position)
{
    if (!isIdle || switchMotor.switchPosition == position)
    {
        delegate->OnMotorControllerSwitchingFinish(position);
        return;
    }

    switchMotor.buffer = SWITCH_RIGHT_STEP - switchMotor.curStep;
    isIdle = false;
}

void MotorController::MoveFilament(
    uint16_t portIndex, uint16_t speed, Direction direction, uint16_t distance)
{
    if (!this->isIdle)
        return;
        
    this->portIndex = portIndex;
    this->speed = speed;
    this->direction = direction;
    this->distance = distance;
    this->pastDistance = 0;

    this->isIdle = false;
}

void MotorController::Stop()
{
    if (this->isIdle)
        return;

    for (uint8_t i = 0; i < 4; i++)
        allMotors[i]->buffer = 0;

    this->isIdle = true;
}

void MotorController::Update(uint32_t time)
{
    if (this->isIdle)
        return;

    if (isHoming)
        this->HomingUpdate(time);
    else
        this->NormalUpdate(time);
}


void MotorController::HomingUpdate(uint32_t time)
{
    switchMotor.buffer -= switchMotor.velocity;
    if (switchMotor.direction)
        switchMotor.curStep += switchMotor.velocity;
    else
        switchMotor.curStep -= switchMotor.velocity;

    if (switchMotor.direction)
    {
        int32_t stallGuard = switchMotor.driver.getStallGuardResult();

        if (stallGuard > 5 && stallGuard < 70 && millis() > switchMotor.directionChangedOn + 1000)
        {
            switchMotor.buffer = SWITCH_HOMING_EXTRA_STEP;
            switchMotor.direction = false;
            switchMotor.driver.enableInverseMotorDirection();
            switchMotor.directionChangedOn = millis();
        }
    }
    else if (switchMotor.buffer == 0)
    {
        switchMotor.driver.moveAtVelocity(0);
        switchMotor.driver.disableInverseMotorDirection();
        switchMotor.switchPosition = SMP_RIGHT;
        isHoming = false;
        this->delegate->OnMotorControllerSwitchingFinish(switchMotor.switchPosition);
    }
}

void MotorController::NormalUpdate(uint32_t time)
{

}