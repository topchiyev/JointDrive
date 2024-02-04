#include "MotorController.h"
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <STM32TimerInterrupt.h>

// 1 STEP = 1.8 DEGREE
// 1 STEP = 8 MICROSTEPS
// 1 MICROSTEP = 0,225 DEGREE
// 360 DEGREE = 200 STEPS
// 360 DEGREE = 1600 MICROSTEPS

#define SWITCH_STEPS_PER_MM         80
#define SWITCH_VREF 800
#define SWITCH_RIGHT_POS           88 * SWITCH_STEPS_PER_MM
#define SWITCH_CENTER_POS          44 * SWITCH_STEPS_PER_MM 
#define SWITCH_LEFT_POS            0
#define SWITCH_HOMING_EXTRA_STEP    20 * SWITCH_STEPS_PER_MM
#define SWITCH_SPEED                40 * SWITCH_STEPS_PER_MM
#define SWITCH_ACCELERATION         1000000000

#define FILAMENT_STEPS_PER_MM       141
#define FILAMENT_VREF 1700
#define FILAMENT_SPEED              140 * FILAMENT_STEPS_PER_MM 
#define FILAMENT_ACCELERATION       1000000000

#define BAUD_RATE 256000
#define R_SENSE 0.11f
#define STALL_THRESHOLD 40 // [0..255]

HardwareSerial Serial4(USART4);

#define R_SENSE 0.11f
#define STALL_VALUE 50 // [0..255]
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
long filament1LastPos = 0;

#define MOTOR_FILAMENT2_EN PB1
#define MOTOR_FILAMENT2_STEP PB0
#define MOTOR_FILAMENT2_DIR PC5
TMC2209Stepper driverFilament2(&Serial4, R_SENSE, 1);
AccelStepper stepperFilament2(AccelStepper::DRIVER, MOTOR_FILAMENT2_STEP, MOTOR_FILAMENT2_DIR);
bool filament2Stopped = true;
long filament2LastPos = 0;

#define MOTOR_FILAMENT3_EN PD1
#define MOTOR_FILAMENT3_STEP PB3
#define MOTOR_FILAMENT3_DIR PB4
TMC2209Stepper driverFilament3(&Serial4, R_SENSE, 3);
AccelStepper stepperFilament3(AccelStepper::DRIVER, MOTOR_FILAMENT3_STEP, MOTOR_FILAMENT3_DIR);
bool filament3Stopped = true;
long filament3LastPos = 0;

#define MOTOR_UPDATE_INTERVAL_MS 400
STM32Timer motorTimer(TIM1);

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
    driverSwitch.rms_current(SWITCH_VREF); // mA
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
    stepperSwitch.disableOutputs();

    driverFilament1.begin();
    driverFilament1.toff(4);
    driverFilament1.blank_time(24);
    driverFilament1.rms_current(FILAMENT_VREF); // mA
    driverFilament1.microsteps(MICROSTEPS);
    driverFilament1.TCOOLTHRS(0xFFFFF); // 20bit max
    driverFilament1.semin(5);
    driverFilament1.semax(2);
    driverFilament1.sedn(0b01);
    driverFilament1.SGTHRS(STALL_VALUE);

    stepperFilament1.setMaxSpeed(SPEED_FILAMENT); // 100mm/s @ 80 steps/mm
    stepperFilament1.setAcceleration(ACCELRATION); // 2000mm/s^2
    stepperFilament1.setEnablePin(MOTOR_FILAMENT1_EN);
    stepperFilament1.setPinsInverted(false, false, true);
    stepperFilament1.disableOutputs();

    driverFilament2.begin();
    driverFilament2.toff(4);
    driverFilament2.blank_time(24);
    driverFilament2.rms_current(FILAMENT_VREF); // mA
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
    stepperFilament2.disableOutputs();

    driverFilament3.begin();
    driverFilament3.toff(4);
    driverFilament3.blank_time(24);
    driverFilament3.rms_current(FILAMENT_VREF); // mA
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
    stepperFilament3.disableOutputs();

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
    bool dir = true;
    if (portIndex == 1 || portIndex == 3 || portIndex == 5)
        dir = false;
    
    if (originalDir == D_BACKWARD)
        dir = !dir;

    return dir;
} 

SwitchMotorPosition MotorController::PortIndexToSwitchPosition(uint8_t portIndex)
{
    switch (portIndex)
    {
        case 1:
        case 3:
        case 5:
            return SMP_LEFT;
        default:
            return SMP_RIGHT;
    }
}

void MotorController::Home()
{
    if (!isIdle)
        return;

    isHoming = true;
    stepperSwitch.enableOutputs();
    stepperSwitch.setCurrentPosition(0);
    stepperSwitch.moveTo(SWITCH_RIGHT_POS + SWITCH_HOMING_EXTRA_STEP);

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
            destPos = SWITCH_LEFT_POS;
            break;
        case SMP_CENTER:
            destPos = SWITCH_CENTER_POS;
            break;
        case SMP_RIGHT:
            destPos = SWITCH_RIGHT_POS;
            break;
    }

    switchStopped = false;
    stepperSwitch.enableOutputs();
    stepperSwitch.moveTo(destPos);

    this->isIdle = false;
}

void MotorController::MoveFilament(uint16_t portIndex, Direction direction, uint32_t distance)
{
    if (!this->isIdle)
        return;
        
    this->portIndex = portIndex;
    this->direction = direction;
    this->distance = distance;

    SwitchMotorPosition destPos = PortIndexToSwitchPosition(this->portIndex);
    if (switchPosition != destPos)
        return;

    uint8_t motorIndex = PortIndexToMotorIndex(portIndex);
    bool dir = PortIndexToDirection(this->portIndex, direction);

    long move = (dir ? 1L : -1L) * distance * FILAMENT_STEPS_PER_MM;

    switch (motorIndex)
    {
        case 1:
            this->isIdle = false;
            filament1Stopped = false;
            filament1LastPos = stepperFilament1.currentPosition();
            stepperFilament1.enableOutputs();
            stepperFilament1.move(move);
            break;
        case 2:
            this->isIdle = false;
            filament2Stopped = false;
            filament2LastPos = stepperFilament2.currentPosition();
            stepperFilament2.enableOutputs();
            stepperFilament2.move(move);
            break;
        case 3:
            this->isIdle = false;
            filament3Stopped = false;
            filament3LastPos = stepperFilament3.currentPosition();
            stepperFilament3.enableOutputs();
            stepperFilament3.move(move);
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

    stepperSwitch.disableOutputs();
    stepperFilament1.disableOutputs();
    stepperFilament2.disableOutputs();
    stepperFilament3.disableOutputs();

    switchStopped = true;
    filament1Stopped = true;
    filament2Stopped = true;
    filament3Stopped = true;
    
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
            if (stepperSwitch.currentPosition() == SWITCH_LEFT_POS)
            {
                switchPosition = SMP_LEFT;
            }
            else if (stepperSwitch.currentPosition() == SWITCH_RIGHT_POS)
            {
                switchPosition = SMP_RIGHT;
            }
            else
            {
                switchPosition = SMP_CENTER;
            }
            this->isIdle = true;
            stepperSwitch.disableOutputs();
            delegate->OnMotorControllerFinishedSwitching(switchPosition);
        }
    }
    else if (!filament1Stopped)
    {
        long pos = stepperFilament1.currentPosition();
        long distGone = (pos - filament1LastPos) / FILAMENT_STEPS_PER_MM;
        long absDistGone = (long)abs(distGone);
        if (absDistGone > 0L)
        {
            filament1LastPos = pos;
            delegate->OnMotorControllerMoved(this->portIndex, (uint32_t)absDistGone, direction);
        }
        if (!stepperFilament1.isRunning())
        {
            filament1Stopped = true;
            this->isIdle = true;
            stepperFilament1.disableOutputs();
            delegate->OnMotorControllerFinishedMoving(this->portIndex);
        }
    }
    else if (!filament2Stopped)
    {
        long pos = stepperFilament2.currentPosition();
        long distGone = (pos - filament2LastPos) / FILAMENT_STEPS_PER_MM;
        long absDistGone = (long)abs(distGone);
        if (absDistGone > 0L)
        {
            filament2LastPos = pos;
            delegate->OnMotorControllerMoved(this->portIndex, (uint32_t)absDistGone, direction);
        }
        if (!stepperFilament2.isRunning())
        {
            filament2Stopped = true;
            this->isIdle = true;
            stepperFilament2.disableOutputs();
            delegate->OnMotorControllerFinishedMoving(this->portIndex);
        }
    }
    else if (!filament3Stopped)
    {
        long pos = stepperFilament3.currentPosition();
        long distGone = (pos - filament3LastPos) / FILAMENT_STEPS_PER_MM;
        long absDistGone = (long)abs(distGone);
        if (absDistGone > 0L)
        {
            filament3LastPos = pos;
            delegate->OnMotorControllerMoved(this->portIndex, (uint32_t)absDistGone, direction);
        }
        if (!stepperFilament3.isRunning())
        {
            filament3Stopped = true;
            this->isIdle = true;
            stepperFilament3.disableOutputs();
            delegate->OnMotorControllerFinishedMoving(this->portIndex);
        }
    }
}

void MotorController::HomingUpdate(uint32_t time)
{
    if (stepperSwitch.targetPosition() == SWITCH_RIGHT_POS + SWITCH_HOMING_EXTRA_STEP)
    {
        sgResult = driverSwitch.SG_RESULT();
        if (stepperSwitch.currentPosition() == SWITCH_RIGHT_POS + SWITCH_HOMING_EXTRA_STEP || (sgResult > 40 && sgResult < 130 && time > switchDirectionChangedOn + 1000))
        {
            stepperSwitch.setCurrentPosition(SWITCH_RIGHT_POS + SWITCH_HOMING_EXTRA_STEP);
            stepperSwitch.moveTo(SWITCH_RIGHT_POS);
            switchDirectionChangedOn = time;
        }
    }
    else if (stepperSwitch.targetPosition() == SWITCH_RIGHT_POS)
    {
        if (stepperSwitch.distanceToGo() == 0)
        {
            stepperSwitch.disableOutputs();
            switchPosition = SMP_RIGHT;
            isHoming = false;
            this->isIdle = true;
            this->delegate->OnMotorControllerFinishedHoming();
        }
    }
}
