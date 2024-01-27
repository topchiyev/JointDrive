#include "PortsView.h"
#include "img/img-back-btn-selected.h"
#include "img/img-back-btn-unselected.h"
#include "img/img-load-btn-selected.h"
#include "img/img-load-btn-unselected.h"
#include "img/img-unload-btn-selected.h"
#include "img/img-unload-btn-unselected.h"
#include "img/img-feed-btn-selected.h"
#include "img/img-feed-btn-unselected.h"
#include "img/img-pull-btn-selected.h"
#include "img/img-pull-btn-unselected.h"
#include "img/img-adjust-btn-selected.h"
#include "img/img-adjust-btn-unselected.h"
#include "img/img-cancel-btn-selected.h"
#include "img/img-cancel-btn-unselected.h"

void PortsView::Begin(JointDrive * jointDrive, uint16_t portIndex)
{
    this->jointDrive = jointDrive;
    this->selectedPortIndex = portIndex;
    this->PortChanged();
}

PortState * PortsView::GetPort()
{
    return this->jointDrive->GetPort(this->selectedPortIndex);
}

String PortStatusToString(PortStatus status)
{
    switch (status)
    {
        case PS_EMPTY:
            return "EMPTY";
        case PS_LOADING:
            return "LOADING";
        case PS_LOADED:
            return "LOADED";
        case PS_UNLOADING:
            return "UNLDING";
        case PS_PUSHING:
            return "PUSHING";
        case PS_FEEDING:
            return "FEEDING";
        case PS_PULLING:
            return "PULLING";
    }

    return "";
};

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

    if (port->status == PS_EMPTY)
    {
        this->activeButtons = new PortsViewButton[4] { PVB_BACK, PVB_PORT, PVB_LOAD, PVB_ADJUST };
        this->activeButtonsLen = 4;
    }
    else if (port->status == PS_LOADING || port->status == PS_UNLOADING || port->status == PS_PUSHING || port->status == PS_PULLING)
    {
        this->activeButtons = new PortsViewButton[1] { PVB_CANCEL };
        this->activeButtonsLen = 1;
    }
    else if (port->status == PS_LOADED)
    {
        this->activeButtons = new PortsViewButton[5] { PVB_BACK, PVB_PORT, PVB_FEED, PVB_UNLOAD, PVB_ADJUST };
        this->activeButtonsLen = 5;
    }
    else if (port->status == PS_FEEDING)
    {
        this->activeButtons = new PortsViewButton[4] { PVB_BACK, PVB_PORT, PVB_PULL, PVB_ADJUST };
        this->activeButtonsLen = 4;
    }

    if (!this->IsButtonActive(this->selectedButton))
        this->selectedButton = this->activeButtons[0];
}

void PortsView::Draw(Canvas * canvas)
{
    canvas->AddText(1, 1, 82, "PICK PORT", C_BLACK, FS_X1, A_CENTER);
    canvas->AddRect(1, 10, 82, 1, C_BLACK);

    if (this->selectedButton == PVB_PORT && (!this->jointDrive->isBlink || !this->isInput))
    {
        canvas->AddRect(1, 13, 42, 11, C_BLACK);
        canvas->AddText(3, 15, 78, String(this->selectedPortIndex), C_WHITE, FS_X1, A_LEFT);
    }
    else
    {
        canvas->AddRect(1, 13, 42, 11, C_BLACK);
        canvas->AddRect(2, 14, 40, 9, C_WHITE);
        canvas->AddText(3, 15, 78, String(this->selectedPortIndex), C_BLACK, FS_X1, A_LEFT);
    }
        
    PortState * port = this->GetPort();
    String status = PortStatusToString(port->status);
    
    if (port->status == PS_LOADING)
        status = " " + String(port->filamentPosition) + "/" + String(this->jointDrive->GetLoadedDistance());
    else if (port->status == PS_UNLOADING)
        status = " " + String(port->filamentPosition) + "/" + String(0);
    else if (port->status == PS_PUSHING)
        status = " " + String(port->filamentPosition) + "/" + String(this->jointDrive->GetLoadedDistance() + this->jointDrive->GetFeedingDistance());
    else if (port->status == PS_PULLING)
        status = " " + String(port->filamentPosition) + "/" + String(this->jointDrive->GetLoadedDistance());
    else
        status = "STATS:" + status;
        
    canvas->AddText(1, 27, 82, status, C_BLACK, FS_X1, A_LEFT);
    
    if (this->IsButtonActive(PVB_BACK))
    {
        if (this->selectedButton == PVB_BACK)
            canvas->AddImage(1, 37, &IMG_BACK_BTN_SELECTED);
        else
            canvas->AddImage(1, 37, &IMG_BACK_BTN_UNSELECTED);
    }
        
    if (this->IsButtonActive(PVB_LOAD))
    {
        if (this->selectedButton == PVB_LOAD)
            canvas->AddImage(42, 37, &IMG_LOAD_BTN_SELECTED);
        else
            canvas->AddImage(42, 37, &IMG_LOAD_BTN_UNSELECTED);
    }
        
    if (this->IsButtonActive(PVB_UNLOAD))
    {
        if (this->selectedButton == PVB_UNLOAD)
            canvas->AddImage(42, 37, &IMG_UNLOAD_BTN_SELECTED);
        else
            canvas->AddImage(42, 37, &IMG_UNLOAD_BTN_UNSELECTED);
    }
        
    if (this->IsButtonActive(PVB_FEED))
    {
        if (this->selectedButton == PVB_FEED)
            canvas->AddImage(19, 37, &IMG_FEED_BTN_SELECTED);
        else
            canvas->AddImage(19, 37, &IMG_FEED_BTN_UNSELECTED);
    }
        
    if (this->IsButtonActive(PVB_PULL))
    {
        if (this->selectedButton == PVB_PULL)
            canvas->AddImage(42, 37, &IMG_PULL_BTN_SELECTED);
        else
            canvas->AddImage(42, 37, &IMG_PULL_BTN_UNSELECTED);
    }
            
    if (this->IsButtonActive(PVB_ADJUST))
    {
        if (this->selectedButton == PVB_ADJUST)
            canvas->AddImage(65, 37, &IMG_ADJUST_BTN_SELECTED);
        else
            canvas->AddImage(65, 37, &IMG_ADJUST_BTN_UNSELECTED);
    }
        
    if (this->IsButtonActive(PVB_CANCEL))
    {
        if (this->selectedButton == PVB_CANCEL)
            canvas->AddImage(50, 37, &IMG_CANCEL_BTN_SELECTED);
        else
            canvas->AddImage(50, 37, &IMG_CANCEL_BTN_UNSELECTED);
    }
}

void PortsView::LeftBtnClick()
{
    if (this->activeButtonsLen == 0)
        return;
    
    if (this->isInput && this->selectedButton == PVB_PORT && this->selectedPortIndex > 1)
        this->selectedPortIndex = this->selectedPortIndex - 1;
    else if (!this->isInput && this->GetPrevActiveButton(this->selectedButton) != PVB_NONE)
        this->selectedButton = this->GetPrevActiveButton(this->selectedButton);
}

void PortsView::RightBtnClick()
{
    if (this->activeButtonsLen == 0)
        return;
    
    if (this->isInput && this->selectedButton == PVB_PORT && this->selectedPortIndex < 5)
        this->selectedPortIndex = this->selectedPortIndex + 1;
    else if (!this->isInput && this->GetNextActiveButton(this->selectedButton) != PVB_NONE)
        this->selectedButton = this->GetNextActiveButton(this->selectedButton);
}
    
void PortsView::ActionBtnClick()
{
    if (this->activeButtonsLen == 0)
        return;
    
    if (this->selectedButton == PVB_PORT)
    {
        if (this->isInput)
        {
            this->isInput = false;
            this->PortChanged();
        }
        else
        {
            this->isInput = true;
        }
    }
            
    else if (this->selectedButton == PVB_BACK)
        this->jointDrive->GoToMainView();
    else if (this->selectedButton == PVB_LOAD)
        this->jointDrive->LoadPort(this->selectedPortIndex);
    else if (this->selectedButton == PVB_UNLOAD)
        this->jointDrive->UnloadPort(this->selectedPortIndex);
    else if (this->selectedButton == PVB_FEED)
        this->jointDrive->PushPort(this->selectedPortIndex);
    else if (this->selectedButton == PVB_PULL)
        this->jointDrive->PullPort(this->selectedPortIndex);
    else if (this->selectedButton == PVB_CANCEL)
        this->jointDrive->CancelTask();
    else if (this->selectedButton == PVB_ADJUST)
        this->jointDrive->GoToPortAdjustView(this->selectedPortIndex);
}

PortsViewButton PortsView::GetPrevActiveButton(PortsViewButton button)
{
    for (uint8_t i = 0; i < this->activeButtonsLen; i++)
    {
        uint8_t i1 = this->activeButtonsLen - 1 - i;
        PortsViewButton btn = this->activeButtons[i1];
        if (btn == button && i1 > 0)
            return this->activeButtons[i1-1];
    }
        
    return PVB_NONE;
}
    
PortsViewButton PortsView::GetNextActiveButton(PortsViewButton button)
{
    for (uint8_t i = 0; i < this->activeButtonsLen; i++)
    {
        PortsViewButton btn = this->activeButtons[i];
        if (btn == button && this->activeButtonsLen > i+1)
            return this->activeButtons[i+1];
    }
        
    return PVB_NONE;
}
