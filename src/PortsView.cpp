#include "PortsView.h"

PortsView::PortsView(JointDrive * jointDrive, uint16_t portIndex)
{
    this->jointDrive = jointDrive;
    this->selectedPortIndex = portIndex;
    this->PortChanged();
}

PortState * PortsView::GetPort()
{
    return this->jointDrive->GetPort(this->selectedPortIndex);
}

bool PortsView::IsButtonActive(PortsViewButton btn)
{
    for (size_t i = 0; i < this->activeButtonsLen; i++)
    {
        if (this->activeButtons[i] == btn)
            return true;
    }

    return false;
}

void PortsView::PortChanged()
{
    PortState * port = this->GetPort();

    if (port->status == EMPTY)
    {
        this->activeButtons = new PortsViewButton[4] { PVB_BACK, PVB_PORT, PVB_LOAD, PVB_ADJUST };
        this->activeButtonsLen = 4;
    }
    else if (port->status == LOADING || port->status == UNLOADING || port->status == PUSHING || port->status == PULLING)
    {
        this->activeButtons = new PortsViewButton[1] { PVB_CANCEL };
        this->activeButtonsLen = 1;
    }
    else if (port->status == LOADED)
    {
        this->activeButtons = new PortsViewButton[5] { PVB_BACK, PVB_PORT, PVB_FEED, PVB_UNLOAD, PVB_ADJUST };
        this->activeButtonsLen = 5;
    }
    else if (port->status == FEEDING)
    {
        this->activeButtons = new PortsViewButton[4] { PVB_BACK, PVB_PORT, PVB_PULL, PVB_ADJUST };
        this->activeButtonsLen = 4;
    }

    if (!this->IsButtonActive(this->selectedButton))
        this->selectedButton = this->activeButtons[0];
}

void PortsView::Draw()
{

}


