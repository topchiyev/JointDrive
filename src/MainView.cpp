#include "MainView.h"
#include "Color.h"
#include "State.h"
#include "FontStyle.h"
#include "FontSize.h"
#include "Align.h"
#include "Images.h"

MainView::MainView(JointDrive * jointDrive)
{
    this->jointDrive = jointDrive;
    this->selectedButton = MVB_PORTS;
}

const Image * MainView::GetPortIcon(PortStatus status, bool isSelected)
{
    switch (status)
    {
        case EMPTY:
            return &IMG_PORT_EMPTY;
        case LOADING:
        case UNLOADING:
            if (isSelected)
                return &IMG_PORT_LOADING;
            else
                return &IMG_PORT_EMPTY;
        case LOADED:
            return &IMG_PORT_LOADED;
        case PULLING:
        case PUSHING:
            if (isSelected)
                return &IMG_PORT_FEEDING;
            else
                return &IMG_PORT_LOADED;
        case FEEDING:
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

void MainView::Draw()
{
    Canvas * canvas = this->jointDrive->GetCanvas();

    PortState * ports = this->jointDrive->GetState()->ports;
    for (size_t i = 0; i < 5; i++)
    {
        PortState port = ports[i];

        bool isTextSelected = false;
        if (port.status == LOADING || port.status == UNLOADING || port.status == PULLING || port.status == PUSHING)
            if (!this->jointDrive->isBlink)
                isTextSelected = true;

        const Image * portIcon = this->GetPortIcon(port.status, this->jointDrive->isBlink);
        const Image * portIndexIcon = this->GetPortIndexIcon(port.index, isTextSelected);

        canvas->AddImage(4 + ((port.index - 1) * (20 + 5)), 2, portIcon);
        canvas->AddImage(4 + ((port.index - 1) * (20 + 5)), 24, portIndexIcon);
            
        if (this->selectedButton == MVB_PORTS)
            canvas->AddImage(2, 52, &IMG_PORTS_BTN_SELECTED);
        else
            canvas->AddImage(2, 52, &IMG_PORTS_BTN_UNSELECTED);
            
        if (this->selectedButton == MVB_SETTINGS)
            canvas->AddImage(114, 52, &IMG_SETTINGS_BTN_SELECTED);
        else
            canvas->AddImage(114, 52, &IMG_SETTINGS_BTN_UNSELECTED);
            
        canvas->AddText(0, 42, 128, this->GetActivity(), WHITE, X1, REGULAR, CENTER);
    }
}

String MainView::GetActivity()
{
    PortState * ports = this->jointDrive->GetState()->ports;
    for (size_t i = 0; i < 5; i++)
    {
        PortState port = ports[i];

        if (port.status == FEEDING)
            return "FEEDING PORT " + port.index;
        if (port.status == PULLING)
            return "PULLING PORT " + port.index;
        if (port.status == LOADING)
            return "LOADING PORT " + port.index;
        if (port.status == UNLOADING)
            return "UNLOADING PORT " + port.index;
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