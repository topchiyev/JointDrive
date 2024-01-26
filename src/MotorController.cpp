#include "MotorController.h"
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <STM32TimerInterrupt.h>

// 1 STEP = 1.8 DEGREE
// 1 STEP = 8 MICROSTEPS
// 1 MICROSTEP = 0,225 DEGREE
// 360 DEGREE = 200 STEPS
// 360 DEGREE = 1600 MICROSTEPS

#define STEPS_PER_MM                80

#define SWITCH_RIGHT_STEP           90 * STEPS_PER_MM
#define SWITCH_CENTER_STEP          45 * STEPS_PER_MM 
#define SWITCH_LEFT_STEP            0
#define SWITCH_HOMING_EXTRA_STEP    10 * STEPS_PER_MM
#define SWITCH_SPEED                40 * STEPS_PER_MM
#define SWITCH_ACCELERATION         1000000000

#define FILAMENT_SPEED              25 * STEPS_PER_MM 
#define FILAMENT_ACCELERATION       1000000000

#define BAUD_RATE 256000
#define R_SENSE 0.11f
#define STALL_THRESHOLD 40 // [0..255]

HardwareSerial Serial4(USART4);

#define R_SENSE 0.11f
#define STALL_VALUE 50 // [0..255]
#define VREF 800
#define SPEED_SWITCH 3000
#define SPEED_FILAMENT 2000
#define ACCELRATION 1000000000
#define MICROSTEPS 16

#define MOTOR_SWITCH_EN PB14
#define MOTOR_SWITCH_STEP PB13
#define MOTOR_SWITCH_DIR PB12
TMC2209Stepper driverSwitch(&Serial4, R_SENSE, 0);
AccelStepper stepperSwitch(AccelStepper::DRIVER, MOTOR_SWITCH_STEP, MOTOR_SWITCH_DIR);
SwitchMotorPosition switchPosition = SMP_UNKNOWN;
uint32_t switchDirectionChangedOn = 0;
bool switchStopped = true;

#define MOTOR_FILAMENT1_EN PB11
#define MOTOR_FILAMENT1_STEP PB10
#define MOTOR_FILAMENT1_DIR PB2
TMC2209Stepper driverFilament1(&Serial4, R_SENSE, 2);
AccelStepper stepperFilament1(AccelStepper::DRIVER, MOTOR_FILAMENT1_STEP, MOTOR_FILAMENT1_DIR); 
bool filament1Stopped = true;

#define MOTOR_FILAMENT2_EN PB1
#define MOTOR_FILAMENT2_STEP PB0
#define MOTOR_FILAMENT2_DIR PC5
TMC2209Stepper driverFilament2(&Serial4, R_SENSE, 1);
AccelStepper stepperFilament2(AccelStepper::DRIVER, MOTOR_FILAMENT2_STEP, MOTOR_FILAMENT2_DIR);
bool filament2Stopped = true;

#define MOTOR_FILAMENT3_EN PD1
#define MOTOR_FILAMENT3_STEP PB3
#define MOTOR_FILAMENT3_DIR PB4
TMC2209Stepper driverFilament3(&Serial4, R_SENSE, 3);
AccelStepper stepperFilament3(AccelStepper::DRIVER, MOTOR_FILAMENT3_STEP, MOTOR_FILAMENT3_DIR);
bool filament3Stopped = true;

#define MOTOR_UPDATE_INTERVAL_MS 400
STM32Timer motorTimer(TIM1);

bool isHoming = false;
uint32_t sgResult = 0;

using namespace TMC2209_n;

void UpdateMotors()
{
    stepperSwitch.run();
    stepperFilament1.run();
    stepperFilament2.run();
    stepperFilament3.run();
}

void MotorController::Begin(MotorControllerDelegate * delegate)
{
    this->delegate = delegate;

    Serial4.begin(256000);

    driverSwitch.beginSerial(256000);

    pinMode(MOTOR_SWITCH_EN, OUTPUT);
    pinMode(MOTOR_SWITCH_STEP, OUTPUT);
    pinMode(MOTOR_SWITCH_DIR, OUTPUT);

    driverFilament1.beginSerial(256000);

    pinMode(MOTOR_FILAMENT1_EN, OUTPUT);
    pinMode(MOTOR_FILAMENT1_STEP, OUTPUT);
    pinMode(MOTOR_FILAMENT1_DIR, OUTPUT);

    driverFilament2.beginSerial(256000);

    pinMode(MOTOR_FILAMENT2_EN, OUTPUT);
    pinMode(MOTOR_FILAMENT2_STEP, OUTPUT);
    pinMode(MOTOR_FILAMENT2_DIR, OUTPUT);

    driverFilament3.beginSerial(256000);

    pinMode(MOTOR_FILAMENT3_EN, OUTPUT);
    pinMode(MOTOR_FILAMENT3_STEP, OUTPUT);
    pinMode(MOTOR_FILAMENT3_DIR, OUTPUT);

    driverSwitch.begin();
    driverSwitch.toff(4);
    driverSwitch.blank_time(24);
    driverSwitch.rms_current(VREF); // mA
    driverSwitch.microsteps(MICROSTEPS);
    driverSwitch.TCOOLTHRS(0xFFFFF); // 20bit max
    driverSwitch.semin(5);
    driverSwitch.semax(2);
    driverSwitch.sedn(0b01);
    driverSwitch.SGTHRS(STALL_VALUE);

    stepperSwitch.setMaxSpeed(SPEED_SWITCH); // 100mm/s @ 80 steps/mm
    stepperSwitch.setAcceleration(ACCELRATION); // 2000mm/s^2
    stepperSwitch.setEnablePin(MOTOR_SWITCH_EN);
    stepperSwitch.setPinsInverted(true, false, true);
    stepperSwitch.enableOutputs();

    driverFilament1.begin();
    driverFilament1.toff(4);
    driverFilament1.blank_time(24);
    driverFilament1.rms_current(VREF); // mA
    driverFilament1.microsteps(MICROSTEPS);
    driverFilament1.TCOOLTHRS(0xFFFFF); // 20bit max
    driverFilament1.semin(5);
    driverFilament1.semax(2);
    driverFilament1.sedn(0b01);
    driverFilament1.SGTHRS(STALL_VALUE);

    stepperFilament1.setMaxSpeed(SPEED_FILAMENT); // 100mm/s @ 80 steps/mm
    stepperFilament1.setAcceleration(ACCELRATION); // 2000mm/s^2
    stepperFilament1.setEnablePin(MOTOR_FILAMENT1_EN);
    stepperFilament1.setPinsInverted(true, false, true);
    stepperFilament1.enableOutputs();

    driverFilament2.begin();
    driverFilament2.toff(4);
    driverFilament2.blank_time(24);
    driverFilament2.rms_current(VREF); // mA
    driverFilament2.microsteps(MICROSTEPS);
    driverFilament2.TCOOLTHRS(0xFFFFF); // 20bit max
    driverFilament2.semin(5);
    driverFilament2.semax(2);
    driverFilament2.sedn(0b01);
    driverFilament2.SGTHRS(STALL_VALUE);

    stepperFilament2.setMaxSpeed(SPEED_FILAMENT); // 100mm/s @ 80 steps/mm
    stepperFilament2.setAcceleration(ACCELRATION); // 2000mm/s^2
    stepperFilament2.setEnablePin(MOTOR_FILAMENT2_EN);
    stepperFilament2.setPinsInverted(false, false, true);
    stepperFilament2.enableOutputs();

    driverFilament3.begin();
    driverFilament3.toff(4);
    driverFilament3.blank_time(24);
    driverFilament3.rms_current(VREF); // mA
    driverFilament3.microsteps(MICROSTEPS);
    driverFilament3.TCOOLTHRS(0xFFFFF); // 20bit max
    driverFilament3.semin(5);
    driverFilament3.semax(2);
    driverFilament3.sedn(0b01);
    driverFilament3.SGTHRS(STALL_VALUE);

    stepperFilament3.setMaxSpeed(SPEED_FILAMENT); // 100mm/s @ 80 steps/mm
    stepperFilament3.setAcceleration(ACCELRATION); // 2000mm/s^2
    stepperFilament3.setEnablePin(MOTOR_FILAMENT3_EN);
    stepperFilament3.setPinsInverted(false, false, true);
    stepperFilament3.enableOutputs();

    motorTimer.attachInterruptInterval(MOTOR_UPDATE_INTERVAL_MS, UpdateMotors);
}

uint8_t PortIndexToMotorIndex(uint8_t portIndex)
{
    switch (portIndex)
    {
        case 1:
            return 1;
        case 2:
        case 3:
            return 2;
        case 4:
        case 5:
            return 3;
        
        default:
            return -1;
    }
}

bool PortIndexToDirection(uint8_t portIndex, Direction originalDir)
{
    switch (portIndex)
    {
        case 1:
        case 3:
        case 5:
            return originalDir == D_FORWARD ? true : false;
        default:
            return originalDir == D_FORWARD ? false : true;
    }
} 

SwitchMotorPosition PortIndexToSwitchPosition(uint8_t portIndex)
{
    switch (portIndex)
    {
        case 1:
        case 3:
        case 5:
            return SMP_RIGHT;
        default:
            return SMP_LEFT;
    }
}

void MotorController::Home()
{
    if (!isIdle)
        return;

    isHoming = true;
    stepperSwitch.setCurrentPosition(0);
    stepperSwitch.moveTo(SWITCH_RIGHT_STEP + SWITCH_HOMING_EXTRA_STEP);

    isIdle = false;
}

SwitchMotorPosition MotorController::GetSwitchPosition()
{
    return switchPosition;
}

uint32_t MotorController::GetSgResult()
{
    return sgResult;
}

void MotorController::SwitchMotorToPosition(SwitchMotorPosition position)
{
    if (!isIdle)
        return;
    
    if (switchPosition == position)
    {
        delegate->OnMotorControllerFinishedSwitching(position);
        return;
    }

    long destPos = 0;

    switch (position)
    {
        case SMP_LEFT:
            destPos = SWITCH_LEFT_STEP;
            break;
        case SMP_CENTER:
            destPos = SWITCH_CENTER_STEP;
            break;
        case SMP_RIGHT:
            destPos = SWITCH_RIGHT_STEP;
            break;
    }

    switchStopped = false;
    stepperSwitch.moveTo(destPos);

    this->isIdle = false;
}

void MotorController::MoveFilament(uint16_t portIndex, Direction direction, uint16_t distance)
{
    if (!this->isIdle)
        return;
        
    this->portIndex = portIndex;

    auto destPos = PortIndexToSwitchPosition(portIndex);
    if (switchPosition != destPos)
        return;

    auto motorIndex = PortIndexToMotorIndex(portIndex);
    auto dir = PortIndexToDirection(portIndex, direction);

    long move = (direction ? 1 : -1) * distance * STEPS_PER_MM;

    switch (motorIndex)
    {
        case 1:
            this->isIdle = true;
            filament1Stopped = false;
            stepperFilament1.moveTo(move);
            break;
        case 2:
            this->isIdle = true;
            filament2Stopped = false;
            stepperFilament2.moveTo(move);
            break;
        case 3:
            this->isIdle = true;
            filament3Stopped = false;
            stepperFilament3.moveTo(move);
            break;
    }
}

void MotorController::Stop()
{
    if (this->isIdle)
        return;

    stepperSwitch.stop();
    stepperFilament1.stop();
    stepperFilament2.stop();
    stepperFilament3.stop();
    
    this->isIdle = true;
}

void MotorController::Update(uint32_t time)
{
    if (this->isIdle)
        return;

    if (isHoming)
    {
        this->HomingUpdate(time);
    }
    else
    {
        this->NormalUpdate(time);
    }
}

void MotorController::NormalUpdate(uint32_t time)
{
    if (!switchStopped)
    {
        if (!stepperSwitch.isRunning())
        {
            switchStopped = true;
            if (stepperSwitch.currentPosition() == SWITCH_LEFT_STEP)
            {
                switchPosition = SMP_LEFT;
            }
            else if (stepperSwitch.currentPosition() == SWITCH_RIGHT_STEP)
            {
                switchPosition = SMP_RIGHT;
            }
            else
            {
                switchPosition = SMP_CENTER;
            }
            delegate->OnMotorControllerFinishedSwitching(switchPosition);
            this->isIdle = true;
        }
    }
    else if (!filament1Stopped)
    {
        if (stepperFilament1.isRunning())
        {
            delegate->OnMotorControllerMoved(portIndex, stepperFilament1.distanceToGo());
        }
        else
        {
            filament1Stopped = true;
            delegate->OnMotorControllerFinishedMoving(portIndex);
            this->isIdle = true;
        }
    }
    else if (!filament2Stopped)
    {
        if (!stepperFilament2.isRunning())
        {
            delegate->OnMotorControllerMoved(portIndex, stepperFilament2.distanceToGo());
        }
        else
        {
            filament2Stopped = true;
            delegate->OnMotorControllerFinishedMoving(portIndex);
            this->isIdle = true;
        }
    }
    else if (!filament3Stopped)
    {
        if (!stepperFilament3.isRunning())
        {
            delegate->OnMotorControllerMoved(portIndex, stepperFilament3.distanceToGo());
        }
        else
        {
            filament3Stopped = true;
            delegate->OnMotorControllerFinishedMoving(portIndex);
            this->isIdle = true;
        }
    }
}

void MotorController::HomingUpdate(uint32_t time)
{
    if (stepperSwitch.targetPosition() == SWITCH_RIGHT_STEP + SWITCH_HOMING_EXTRA_STEP)
    {
        sgResult = driverSwitch.SG_RESULT();
        if (sgResult > 20 && sgResult < 125 && time > switchDirectionChangedOn + 1000)
        {
            stepperSwitch.setCurrentPosition(SWITCH_RIGHT_STEP + SWITCH_HOMING_EXTRA_STEP);
            stepperSwitch.moveTo(SWITCH_RIGHT_STEP);
            switchDirectionChangedOn = time;
        }
    }
    else if (stepperSwitch.targetPosition() == SWITCH_RIGHT_STEP)
    {
        if (stepperSwitch.distanceToGo() == 0)
        {
            stepperSwitch.stop();
            switchPosition = SMP_RIGHT;
            isHoming = false;
            this->isIdle = true;
            this->delegate->OnMotorControllerFinishedHoming();
        }
    }
}
