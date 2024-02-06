#include "JointDrive.h"
#include "Storage.h"
#include "Direction.h"

#include "ViewType.h"
#include "IntroView.h"
#include "MainView.h"
#include "SettingsView.h"
#include "PortsView.h"
#include "PortAdjustView.h"

Storage storage;
JointDriveState state;
Canvas canvas;
MotorController motorController;
RotaryEncoderController rotaryInputController;
SwitchInputController switchInputController;

IntroView introView;
MainView mainView;
SettingsView settingsView;
PortsView portsView;
PortAdjustView portAdjustView;
ViewType currentViewType = VT_NONE;

const uint32_t blinkInterval = 500;
uint32_t blinkChangeTime = 0;
bool isSwitchingPorts = false;
uint8_t switchingFromPort = 0;
uint8_t switchingToPort = 0;

void JointDrive::Begin()
{
    canvas.Begin();
    storage.Begin();
    motorController.Begin(this);
    rotaryInputController.Begin(PD0, PD2, PD3, this);
    switchInputController.Begin(this);

    state = storage.GetState();
    if (!state.isInitialized)
    {
        state.feedingDistance = 100;
        state.ports[0].index = 1;
        state.ports[0].status = PS_EMPTY;
        state.ports[1].index = 2;
        state.ports[1].status = PS_EMPTY;
        state.ports[2].index = 3;
        state.ports[2].status = PS_EMPTY;
        state.ports[3].index = 4;
        state.ports[3].status = PS_EMPTY;
        state.ports[4].index = 5;
        state.ports[4].status = PS_EMPTY;

        state.isInitialized = true;
        storage.SetState(state);
        state = state;
    }

    this->GoToIntroView();
}

Canvas * JointDrive::GetCanvas()
{
    return &canvas;
}

JointDriveState * JointDrive::GetState()
{
    return &state;
}

uint32_t JointDrive::GetFeedingDistance()
{
    return this->GetState()->feedingDistance;
}

void JointDrive::SetFeedingDistance(uint32_t value)
{
    state.feedingDistance = value;
    storage.SetState(state);
}

uint32_t JointDrive::GetLoadedDistance()
{
    return 220;
}

PortState * JointDrive::GetPort(uint16_t portIndex)
{
    for (size_t i = 0; i < 5; i++)
    {
        PortState port = state.ports[i];
        if (port.index == portIndex)
        {
            return &state.ports[i];
        }
    }
    
    return nullptr;
}

bool JointDrive::IsPortBusy(uint16_t portIndex, bool includeFeeding)
{
    PortState * port = this->GetPort(portIndex);

    if (port->status == PS_PUSHING or port->status == PS_PULLING)
        return true;
    else if (port->status == PS_LOADING or port->status == PS_UNLOADING)
        return true;
    if (includeFeeding and port->status == PS_FEEDING)
        return true;

    return false;
}

uint32_t JointDrive::GetSgResult()
{
    return motorController.GetSgResult();
}

bool JointDrive::IsFeeding()
{
    for (size_t i = 0; i < 5; i++)
    {
        PortState * port = GetPort(i);
        if (port->status == PS_FEEDING || port->status == PS_PUSHING || port->status == PS_PULLING)
            return true;
    }
    
    return false;
}

void(* resetFunc) (void) = 0;

void JointDrive::ResetState()
{
    storage.ClearState();
    NVIC_SystemReset();
}

void JointDrive::Refresh()
{
    this->Update();
    this->Draw();
}

void JointDrive::Update()
{
    uint32_t curTime = millis();
    if (curTime > blinkChangeTime)
    {
        this->isBlink = !this->isBlink;
        blinkChangeTime = curTime + blinkInterval;
    }

    switch (currentViewType)
    {
        case VT_INTRO:
            introView.Update(curTime);
            break;
    }

    rotaryInputController.Update(curTime);
    switchInputController.Update(curTime);
    motorController.Update(curTime);
}

void JointDrive::Draw()
{
    canvas.Clear();
    
    switch (currentViewType)
    {
        case VT_INTRO:
            introView.Draw(&canvas);
            break;
        case VT_MAIN:
            mainView.Draw(&canvas);
            break;
        case VT_SETTINGS:
            settingsView.Draw(&canvas);
            break;
        case VT_PORTS:
            portsView.Draw(&canvas);
            break;
        case VT_PORT_ADJUST:
            portAdjustView.Draw(&canvas);
            break;
    }

    canvas.Draw();
}


void JointDrive::GoToIntroView()
{
    currentViewType = VT_INTRO;
    introView.Begin(this);
}

void JointDrive::IntroViewFinished()
{
    this->isHoming = true;
    motorController.Home();
    this->GoToMainView();
}

void JointDrive::GoToMainView()
{
    currentViewType = VT_MAIN;
    mainView.Begin(this);
}

void JointDrive::GoToSettingsView()
{
    currentViewType = VT_SETTINGS;
    settingsView.Begin(this);
}

void JointDrive::GoToPortsView(uint16_t portIndex)
{
    currentViewType = VT_PORTS;
    portsView.Begin(this, portIndex);
}

void JointDrive::GoToPortAdjustView(uint16_t portIndex)
{
    currentViewType = VT_PORT_ADJUST;
    portAdjustView.Begin(this, portIndex);
}

void JointDrive::SwitchToPositionForPort(uint16_t portIndex)
{
    if (motorController.isIdle == false)
        return;

    for (uint16_t i = 1; i <= 5; i++)
    {
        PortState * port = GetPort(i);
        if (IsPortBusy(port->index))
            return;
    }

    SwitchMotorPosition desiredPos = motorController.PortIndexToSwitchPosition(portIndex);
    motorController.SwitchMotorToPosition(desiredPos);
}

void JointDrive::SwitchToFreePosition()
{
    motorController.SwitchMotorToPosition(SMP_CENTER);
}

bool JointDrive::IsSwitching()
{
    return isSwitchingPorts;
}

void JointDrive::LoadPort(uint16_t portIndex)
{
    if (motorController.isIdle == false)
        return;
    
    for (uint16_t i = 1; i <= 5; i++)
    {
        if (i == portIndex)
            continue;

        PortState * port = GetPort(i);
        if (IsPortBusy(port->index))
            return;
    }

    PortState * port = GetPort(portIndex);
    
    if (port->status != PS_EMPTY)
        return;
        
    port->status = PS_LOADING;

    if (currentViewType == VT_PORTS)
        portsView.PortChanged();

    SwitchMotorPosition desiredPos = motorController.PortIndexToSwitchPosition(portIndex);
    if (motorController.GetSwitchPosition() != desiredPos)
        return;

    uint32_t dist = this->GetLoadedDistance();
    motorController.MoveFilament(portIndex, D_FORWARD, dist);
}

void JointDrive::UnloadPort(uint16_t portIndex)
{
    for (uint16_t i = 1; i <= 5; i++)
    {
        PortState * port = GetPort(i);
        if (IsPortBusy(port->index))
            return;
    }

    PortState * port = GetPort(portIndex);
        
    if (port->status != PS_LOADED)
        return;
        
    port->status = PS_UNLOADING;

    if (currentViewType == VT_PORTS)
        portsView.PortChanged();

    SwitchMotorPosition desiredPos = motorController.PortIndexToSwitchPosition(portIndex);
    if (motorController.GetSwitchPosition() != desiredPos)
        return;

    uint32_t dist = port->filamentPosition;
    motorController.MoveFilament(portIndex, D_BACKWARD, dist);
}

void JointDrive::PushPort(uint16_t portIndex)
{
    for (uint16_t i = 1; i <= 5; i++)
    {
        PortState * port = GetPort(i);
        if (IsPortBusy(port->index))
            return;
    }

    PortState * port = GetPort(portIndex);
    
    if (port->status != PS_LOADED)
        return;
        
    port->status = PS_PUSHING;
    
    if (currentViewType == VT_PORTS)
        portsView.PortChanged();
        
    uint32_t dist = this->GetFeedingDistance();
    motorController.MoveFilament(portIndex, D_FORWARD, dist);
}

void JointDrive::PullPort(uint16_t portIndex)
{
    for (uint16_t i = 1; i <= 5; i++)
    {
        PortState * port = GetPort(i);
        if (port->index == portIndex)
            continue;
        if (IsPortBusy(port->index))
            return;
    }

    PortState * port = GetPort(portIndex);
    
    if (port->status != PS_FEEDING)
        return;
        
    port->status = PS_PULLING;
    
    if (currentViewType == VT_PORTS)
        portsView.PortChanged();
        
    uint32_t dist = this->GetFeedingDistance();
    motorController.MoveFilament(portIndex, D_BACKWARD, dist);
}

void JointDrive::AdjustPort(uint16_t portIndex, Direction direction)
{
    motorController.MoveFilament(portIndex, direction, 1);
}

void JointDrive::CancelTask()
{
    if (!motorController.isIdle)
        motorController.Stop();
    
    for (uint8_t i = 0; i < 5; i++)
    {
        PortState * port = GetPort(i);
        if (port->status == PS_LOADING or port->status == PS_UNLOADING)
            port->status = PS_LOADED;
        if (port->status == PS_PUSHING || port->status == PS_PULLING)
            port->status = PS_FEEDING;
    }
    
    storage.SetState(state);
    
    if (currentViewType == VT_PORTS)
        portsView.PortChanged();
}



void JointDrive::OnMotorControllerFinishedHoming()
{
    if (this->isHoming)
        this->isHoming = false;

    for (uint8_t i = 0; i < 5; i++)
    {
        PortState * port = GetPort(i+1);
        if (port->status == PS_FEEDING)
        {
            SwitchToFreePosition();
            break;
        }
    }
}
void JointDrive::OnMotorControllerFinishedSwitching(SwitchMotorPosition position)
{
    if (currentViewType == VT_PORTS)
        portsView.SwitchingFinished();
    else if (currentViewType == VT_PORT_ADJUST)
        portAdjustView.SwitchingFinished();

    if (isSwitchingPorts)
    {
        if (switchingFromPort == 0)
        {
            PushPort(switchingToPort);
            isSwitchingPorts = false;
            switchingToPort = 0;
        }
        else
        {
            SwitchMotorPosition fromPos = motorController.PortIndexToSwitchPosition(switchingFromPort);
            SwitchMotorPosition toPos = motorController.PortIndexToSwitchPosition(switchingToPort);
            if (fromPos == position)
            {
                PullPort(switchingFromPort);
            }
            else
            {
                PushPort(switchingToPort);
                isSwitchingPorts = false;
                switchingFromPort = 0;
                switchingToPort = 0;
            }
        }
    }
}
void JointDrive::OnMotorControllerMoved(uint16_t portIndex, uint32_t distanceGone, Direction direction)
{
    PortState * port = this->GetPort(portIndex);
    if (direction == D_FORWARD)
    {
        port->filamentPosition += distanceGone;
    }
    else
    {
        port->filamentPosition = (uint32_t)max(0L, (long)port->filamentPosition - (long)distanceGone);
    }
}
void JointDrive::OnMotorControllerFinishedMoving(uint16_t portIndex)
{
    PortState * port = this->GetPort(portIndex);

    PortStatus oldStatus = port->status;

    if (port->status == PS_LOADING)
    {
        port->status = PS_LOADED;
        port->filamentPosition = this->GetLoadedDistance();
    }
    else if (port->status == PS_UNLOADING)
    {
        port->status = PS_EMPTY;
        port->filamentPosition = 0;
    }
    else if (port->status == PS_PUSHING)
    {
        port->status = PS_FEEDING;
        port->filamentPosition = this->GetLoadedDistance() + this->GetFeedingDistance();
        motorController.SwitchMotorToPosition(SMP_CENTER);
    }
    else if (port->status == PS_PULLING)
    {
        port->status = PS_LOADED;
        port->filamentPosition = this->GetLoadedDistance();
    }
    storage.SetState(state);

    if (currentViewType == VT_PORTS)
    {
        portsView.PortChanged();
    }

    if (isSwitchingPorts && oldStatus == PS_PULLING)
    {
        bool switchingNeeded = false;
        if (switchingFromPort != 0)
        {
            SwitchMotorPosition fromPos = motorController.PortIndexToSwitchPosition(switchingFromPort);
            SwitchMotorPosition toPos = motorController.PortIndexToSwitchPosition(switchingToPort);
            if (fromPos != toPos)
                switchingNeeded = true;
        }
        if (switchingNeeded)
        {
            this->SwitchToPositionForPort(switchingToPort);
        }
        else
        {
            this->PushPort(switchingToPort);
            isSwitchingPorts = false;
            switchingToPort = 0;
            switchingFromPort = 0;
        }
    }
}

void JointDrive::OnRotaryEncoderIncreased()
{
    switch (currentViewType)
    {
        case VT_MAIN:
            mainView.RightBtnClick();
            break;
        case VT_SETTINGS:
            settingsView.RightBtnClick();
            break;
        case VT_PORTS:
            portsView.RightBtnClick();
            break;
        case VT_PORT_ADJUST:
            portAdjustView.RightBtnClick();
            break;
    }
}
void JointDrive::OnRotaryEncoderDecreased()
{
    switch (currentViewType)
    {
        case VT_MAIN:
            mainView.LeftBtnClick();
            break;
        case VT_SETTINGS:
            settingsView.LeftBtnClick();
            break;
        case VT_PORTS:
            portsView.LeftBtnClick();
            break;
        case VT_PORT_ADJUST:
            portAdjustView.LeftBtnClick();
            break;
    }
}
void JointDrive::OnRotaryEncoderPressed()
{
    switch (currentViewType)
    {
        case VT_MAIN:
            mainView.ActionBtnClick();
            break;
        case VT_SETTINGS:
            settingsView.ActionBtnClick();
            break;
        case VT_PORTS:
            portsView.ActionBtnClick();
            break;
        case VT_PORT_ADJUST:
            portAdjustView.ActionBtnClick();
            break;
    }
}

void JointDrive::OnSwitchFilamentChageRequest(uint16_t portIndex)
{
    PortState * toPort = this->GetPort(portIndex);
    if (toPort == nullptr)
        return;
    if (toPort->status != PS_LOADED)
        return;

    PortState * fromPort = nullptr;
    for (uint8_t i = 0; i < 5; i++)
    {
        PortState * p = this->GetPort(i);
        if (p->status == PS_FEEDING)
        {
            fromPort = p;
            break;
        }
    }

    isSwitchingPorts = true;
    switchingFromPort = fromPort == nullptr ? 0 : fromPort->index;
    switchingToPort = portIndex;

    if (fromPort != nullptr)
        this->SwitchToPositionForPort(fromPort->index);
    else
        this->SwitchToPositionForPort(toPort->index);
}
bool JointDrive::IsSwitchPressed()
{
    return switchInputController.IsPressed();
}
bool JointDrive::IsSwitchPulse()
{
    return switchInputController.IsPulse();
}