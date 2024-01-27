#include "JointDrive.h"
#include "Storage.h"
#include "Direction.h"

#include "ViewType.h"
#include "IntroView.h"
#include "MainView.h"
#include "SettingsView.h"
#include "PortsView.h"
#include "PortAdjustView.h"

enum JointDriveActionType
{
    JDAT_SWITCH = 0,
    JDAT_LOAD = 1,
    JDAT_UNLOAD = 2,
    JDAT_PUSH = 3,
    JDAT_PULL = 4
};

struct JointDriveAction
{
    bool isEmpty = true;
    bool isWorking = false;
    JointDriveActionType type;
    SwitchMotorPosition switchPosition;
    uint8_t portIndex;
    Direction direction;
    uint32_t distance;
    bool isAwaitingHuman;
};

Storage storage;
JointDriveState state;
Canvas canvas;
MotorController motorController;
RotaryEncoderController inputController;

IntroView introView;
MainView mainView;
SettingsView settingsView;
PortsView portsView;
PortAdjustView portAdjustView;
ViewType currentViewType = VT_NONE;

const uint32_t blinkInterval = 500;
uint32_t blinkChangeTime = 0;

JointDriveAction actions[4];
uint8_t currentActionIndex = 0;

void JointDrive::Begin()
{
    canvas.Begin();
    storage.Begin();
    inputController.Begin(PD0, PD2, PD3, this);
    motorController.Begin(this);

    // To reset state
    storage.ClearState();
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

    this->UpdateActions();

    this->MakePortAdjustmentMove();

    inputController.Update(curTime);
    motorController.Update(curTime);
}

void JointDrive::UpdateActions()
{
    JointDriveAction * action = &actions[currentActionIndex];
    if (action->isEmpty)
    {
        currentActionIndex++;
        if (currentActionIndex > 3)
            currentActionIndex = 0;
    }
    else if (!action->isWorking)
    {
        if (action->type == JDAT_SWITCH)
        {
            action->isWorking = true;
            motorController.SwitchMotorToPosition(action->switchPosition);
        }
        else if (action->type == JDAT_LOAD)
        {
            if (!action->isAwaitingHuman)
            {
                action->isWorking = true;
                action->isAwaitingHuman = true;
            }
            else
            {
                motorController.MoveFilament(action->portIndex, action->direction, action->distance);
            }
        }
    }
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

void JointDrive::LoadPort(uint16_t portIndex)
{
    if (motorController.isIdle == false)
        return;
    
    for (uint16_t i = 1; i <= 5; i++)
    {
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
        
    uint32_t dist = this->GetLoadedDistance();
    // if (port->filamentPosition > 0)
    //     dist -= port->filamentPosition;

    SwitchMotorPosition desiredPos = motorController.PortIndexToSwitchPosition(portIndex);
    
    currentActionIndex = 0;
    uint8_t i = 0;

    if (motorController.GetSwitchPosition() != desiredPos)
    {
        actions[i].type = JDAT_SWITCH;
        actions[i].switchPosition = desiredPos;
        actions[i].isEmpty = false;
        actions[i].isWorking = false;
        i++;
    }

    actions[i].type = JDAT_LOAD;
    actions[i].portIndex = portIndex;
    actions[i].direction = D_FORWARD;
    actions[i].distance = dist;
    actions[i].isEmpty = false;
    actions[i].isWorking = false;
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
    if (port->filamentPosition > this->GetLoadedDistance())
        dist -= (port->filamentPosition - this->GetLoadedDistance());
    motorController.MoveFilament(portIndex, D_FORWARD, dist);
}

void JointDrive::PullPort(uint16_t portIndex)
{
    for (uint16_t i = 1; i <= 5; i++)
    {
        PortState * port = GetPort(i);
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
    // if (filammentMoveBufferDirection != direction)
    //     filamentMoveBuffer = 0;
    // filammentMoveBufferDirection = direction;
    // filamentMoveBufferPortIndex = portIndex;
    // filamentMoveBuffer++;
}

void JointDrive::MakePortAdjustmentMove()
{
    // if (filamentMoveBuffer == 0 || filamentMoveBufferPortIndex == 0)
    //     return;
    
    // filamentMoveBuffer--;
    // motorController.MoveFilament(filamentMoveBufferPortIndex, 1, filammentMoveBufferDirection, 1);
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
}
void JointDrive::OnMotorControllerFinishedSwitching(SwitchMotorPosition position)
{
    JointDriveAction * action = &actions[currentActionIndex];
    action->isEmpty = true;
    action->isWorking = false;
}
void JointDrive::OnMotorControllerMoved(uint16_t portIndex, uint32_t distanceLeft)
{
    JointDriveAction * action = &actions[currentActionIndex];

    this->GetPort(portIndex)->filamentPosition = action->distance - distanceLeft;
}
void JointDrive::OnMotorControllerFinishedMoving(uint16_t portIndex)
{
    PortState * port = this->GetPort(portIndex);
    if (port->status == PS_LOADING)
    {
        port->status = PS_LOADED;
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
    }
    else if (port->status == PS_PULLING)
    {
        port->status = PS_LOADED;
    }
    storage.SetState(state);

    if (currentViewType == VT_PORTS)
    {
        portsView.PortChanged();
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