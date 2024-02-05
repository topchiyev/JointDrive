#include "MainView.h"
#include "Color.h"
#include "State.h"
#include "FontSize.h"
#include "Align.h"
#include "img/img-port-empty.h"
#include "img/img-port-loaded.h"
#include "img/img-port-loading.h"
#include "img/img-port-feeding.h"
#include "img/img-port-number-1-selected.h"
#include "img/img-port-number-2-selected.h"
#include "img/img-port-number-3-selected.h"
#include "img/img-port-number-4-selected.h"
#include "img/img-port-number-5-selected.h"
#include "img/img-port-number-1-unselected.h"
#include "img/img-port-number-2-unselected.h"
#include "img/img-port-number-3-unselected.h"
#include "img/img-port-number-4-unselected.h"
#include "img/img-port-number-5-unselected.h"
#include "img/img-ports-btn-selected.h"
#include "img/img-settings-btn-selected.h"
#include "img/img-ports-btn-unselected.h"
#include "img/img-settings-btn-unselected.h"

void MainView::Begin(JointDrive * jointDrive)
{
    this->jointDrive = jointDrive;
    this->selectedButton = MVB_PORTS;
}

const Image * MainView::GetPortIcon(PortStatus status, bool isSelected)
{
    switch (status)
    {
        case PS_EMPTY:
            return &IMG_PORT_EMPTY;
        case PS_LOADING:
        case PS_UNLOADING:
            if (isSelected)
                return &IMG_PORT_LOADING;
            else
                return &IMG_PORT_EMPTY;
        case PS_LOADED:
            return &IMG_PORT_LOADED;
        case PS_PULLING:
        case PS_PUSHING:
            if (isSelected)
                return &IMG_PORT_FEEDING;
            else
                return &IMG_PORT_LOADED;
        case PS_FEEDING:
            return &IMG_PORT_FEEDING;
    }

    return nullptr;
}

const Image * MainView::GetPortIndexIcon(uint16_t index, bool isSelected)
{
    if (isSelected)
    {
        switch (index)
        {
            case 1:
                return &IMG_PORT_NUMBER_1_SELECTED;
            case 2:
                return &IMG_PORT_NUMBER_2_SELECTED;
            case 3:
                return &IMG_PORT_NUMBER_3_SELECTED;
            case 4:
                return &IMG_PORT_NUMBER_4_SELECTED;
            case 5:
                return &IMG_PORT_NUMBER_5_SELECTED;
        }
    }
    else
    {
        switch (index)
        {
            case 1:
                return &IMG_PORT_NUMBER_1_UNSELECTED;
            case 2:
                return &IMG_PORT_NUMBER_2_UNSELECTED;
            case 3:
                return &IMG_PORT_NUMBER_3_UNSELECTED;
            case 4:
                return &IMG_PORT_NUMBER_4_UNSELECTED;
            case 5:
                return &IMG_PORT_NUMBER_5_UNSELECTED;
        }
    }

    return nullptr;
}

void MainView::Draw(Canvas * canvas)
{
    PortState * ports = this->jointDrive->GetState()->ports;
    for (size_t i = 0; i < 5; i++)
    {
        PortState port = ports[i];

        bool isTextSelected = false;
        if (port.status == PS_LOADING || port.status == PS_UNLOADING || port.status == PS_PULLING || port.status == PS_PUSHING)
            if (!this->jointDrive->isBlink)
                isTextSelected = true;

        const Image * portIcon = this->GetPortIcon(port.status, isTextSelected);
        const Image * portIndexIcon = this->GetPortIndexIcon(port.index, isTextSelected);

        canvas->AddImage(1 + ((port.index - 1) * (14 + 3)), 1, portIcon);
        canvas->AddImage(1 + ((port.index - 1) * (14 + 3)), 17, portIndexIcon);
            
        if (this->selectedButton == MVB_PORTS)
            canvas->AddImage(1, 37, &IMG_PORTS_BTN_SELECTED);
        else
            canvas->AddImage(1, 37, &IMG_PORTS_BTN_UNSELECTED);
            
        if (this->selectedButton == MVB_SETTINGS)
            canvas->AddImage(71, 37, &IMG_SETTINGS_BTN_SELECTED);
        else
            canvas->AddImage(71, 37, &IMG_SETTINGS_BTN_UNSELECTED);
            
        canvas->AddText(0, 27, 84, this->GetActivity(), C_BLACK, FS_X1, A_CENTER);

        canvas->AddRect((84 - 14) / 2, 48 - 8, 14, 8, C_BLACK);
        canvas->AddRect((84 - 10) / 2, 48 - 6, 10, 4, C_WHITE);
        if (jointDrive->IsSwitchPressed())
        {
            if (jointDrive->IsSwitchPulse())
            {
                canvas->AddRect((84 - 10) / 2, 48 - 6, 5, 4, C_BLACK);
            }
            else
            {
                canvas->AddRect(((84 - 10) / 2) + 5, 48 - 6, 5, 4, C_BLACK);
            }
        }
    }
}

String MainView::GetActivity()
{
    if (jointDrive->isHoming)
    {
        //return "HOMING";
        return String(jointDrive->GetSgResult());
    }
    
    PortState * ports = this->jointDrive->GetState()->ports;
    for (size_t i = 0; i < 5; i++)
    {
        PortState port = ports[i];

        if (port.status == PS_FEEDING)
            return "FEEDING " + String(port.index);
        if (port.status == PS_PULLING)
            return "PULLING " + String(port.index);
        if (port.status == PS_LOADING)
            return "LOADING " + String(port.index);
        if (port.status == PS_UNLOADING)
            return "UNLOADING " + String(port.index);
    }
        
    return "IDLE";
}

void MainView::LeftBtnClick()
{
    if (this->selectedButton == MVB_PORTS)
        return;
    
    this->selectedButton = (MainViewButton)(this->selectedButton - 1);
}

void MainView::RightBtnClick()
{
    if (this->selectedButton == MVB_SETTINGS)
        return;
    
    this->selectedButton = (MainViewButton)(this->selectedButton + 1);
}

void MainView::ActionBtnClick()
{
    if (this->selectedButton == MVB_SETTINGS)
        this->jointDrive->GoToSettingsView();
    if (this->selectedButton == MVB_PORTS)
        this->jointDrive->GoToPortsView();
}