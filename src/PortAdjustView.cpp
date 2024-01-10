#include "PortAdjustView.h"
#include "Images.h"
#include "Direction.h"

PortAdjustView::PortAdjustView(JointDrive * jointDrive, uint16_t portIndex)
{
    this->jointDrive = jointDrive;
    this->portIndex = portIndex;
}

PortState * PortAdjustView::GetPort()
{
    return this->jointDrive->GetPort(this->portIndex);
}

void PortAdjustView::Draw()
{
    Canvas * canvas = this->jointDrive->GetCanvas();

    canvas->AddText(2, -3, 128-4, "PORT " + this->portIndex, WHITE, X2, REGULAR, CENTER);
    canvas->AddRect(1, 13, 128-1, 14, WHITE);
    canvas->AddText(2, 19, 128-4, "ADJUST FILAMENT POSITION", WHITE, X1, REGULAR, LEFT);
    
    if (this->selectedButton == PAVB_POSITION && (!this->jointDrive->isBlink || !this->isInput))
    {
        canvas->AddRect(2, 32, 2 + 30, 33 + 12, WHITE);
        canvas->AddText(3, 29, 30, "" + this->GetPort()->filamentPosition, BLACK, X2, REGULAR, CENTER);
    }
    else
    {
        canvas->AddText(3, 29, 30, "" + this->GetPort()->filamentPosition, WHITE, X2, REGULAR, CENTER);
        canvas->AddRect(2, 45, 2 + 30, 45+1, WHITE);
    }

    if (this->selectedButton == PAVB_BACK)
        canvas->AddImage(2, 52, &IMG_BACK_BTN_SELECTED);
    else
        canvas->AddImage(2, 52, &IMG_BACK_BTN_UNSELECTED);
}

void PortAdjustView::LeftBtnClick()
{
    if (this->isInput && this->selectedButton == PAVB_POSITION)
        this->jointDrive->AdjustPort(this->portIndex, BACKWARD);
    else if (!this->isInput && this->selectedButton > PAVB_BACK)
        this->selectedButton = (PortAdjustViewButton)(this->selectedButton - 1);
}

void PortAdjustView::RightBtnClick()
{
    if (this->isInput && this->selectedButton == PAVB_POSITION)
        this->jointDrive->AdjustPort(this->portIndex, FORWARD);
    else if (!this->isInput && this->selectedButton < PAVB_POSITION)
        this->selectedButton = (PortAdjustViewButton)(this->selectedButton + 1);
}

void PortAdjustView::ActionBtnClick()
{
    if (this->selectedButton == PAVB_POSITION)
        this->isInput = !this->isInput;   
    else if (this->selectedButton == PAVB_BACK)
        this->jointDrive->GoToPortsView(this->portIndex);
}