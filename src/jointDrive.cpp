#include "JointDrive.h"
#include "Storage.h"
#include "Direction.h"
#include "View.h"
#include "IntroView.h"
#include "MainView.h"
#include "SettingsView.h"
#include "PortsView.h"
#include "PortAdjustView.h"

Storage storage;
JointDriveState state;
Canvas canvas;
MotorController motorController;
View * currentView = nullptr;
const uint32_t blinkInterval = 100;
uint32_t blinkChangeTime = 0;

JointDrive::JointDrive()
{
    state = storage.GetState();
    if (!state.isInitialized)
    {
        state.feedingDistance = 100;
        state.ports[0].index = 1;
        state.ports[1].index = 2;
        state.ports[2].index = 3;
        state.ports[3].index = 4;
        state.ports[4].index = 5;
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
    return 200;
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

    if (port->status == PUSHING or port->status == PULLING)
        return true;
    else if (port->status == LOADING or port->status == UNLOADING)
        return true;
    if (includeFeeding and port->status == FEEDING)
        return true;

    return false;
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

    currentView->Update(curTime);    
    motorController.Update(curTime);
}

void JointDrive::Draw()
{
    canvas.Clear();
    if (currentView != nullptr)
        currentView->Draw();
    canvas.Draw();
}


void JointDrive::GoToIntroView()
{
    if (currentView != nullptr)
        delete currentView;

    currentView = new IntroView(this);
}

void JointDrive::GoToMainView()
{
    if (currentView != nullptr)
        delete currentView;

    currentView = new MainView(this);
}

void JointDrive::GoToSettingsView()
{
    if (currentView != nullptr)
        delete currentView;

    currentView = new SettingsView(this);
}

void JointDrive::GoToPortsView(uint16_t portIndex)
{
    if (currentView != nullptr)
        delete currentView;

    currentView = new PortsView(this, portIndex);
}

void JointDrive::GoToPortAdjustView(uint16_t portIndex)
{
    if (currentView != nullptr)
        delete currentView;

    currentView = new PortAdjustView(this, portIndex);
}


void JointDrive::LoadPort(uint16_t portIndex)
{
    for (uint16_t i = 1; i <= 5; i++)
    {
        PortState * port = GetPort(i);
        if (IsPortBusy(port->index))
            return;
    }

    PortState * port = GetPort(portIndex);
    
    if (port->status != EMPTY)
        return;
        
    port->status = LOADING;

    if (currentView->viewType == V_PORTS)
        currentView->PortChanged();
        
    uint32_t dist = this->GetLoadedDistance();
    if (port->filamentPosition > 0)
        dist -= port->filamentPosition;
    
    motorController.MoveFilament(port->index, 1, D_FORWARD, dist, this);
}

void JointDrive::UnloadPort(uint16_t portIndex)
{

}

void JointDrive::PushPort(uint16_t portIndex)
{

}

void JointDrive::PullPort(uint16_t portIndex)
{

}

void JointDrive::AdjustPort(uint16_t portIndex, Direction direction)
{

}

void JointDrive::CancelTask()
{

}
void JointDrive::OnMotorControllerStep(uint16_t portIndex, uint32_t distance)
{

}

void JointDrive::OnMotorControllerFinish(uint16_t portIndex)
{

}