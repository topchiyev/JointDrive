#include "PortAdjustView.h"
#include "Direction.h"
#include "img/img-back-btn-selected.h"
#include "img/img-back-btn-unselected.h"

void PortAdjustView::Begin(JointDrive * jointDrive, uint16_t portIndex)
{
    this->jointDrive = jointDrive;
    this->portIndex = portIndex;
}

PortState * PortAdjustView::GetPort()
{
    return this->jointDrive->GetPort(this->portIndex);
}

void PortAdjustView::Draw(Canvas * canvas)
{
    canvas->AddText(1, 1, 82, "PORT " + String(this->portIndex), C_BLACK, FS_X1, A_CENTER);
    canvas->AddText(1, 11, 82, "FILAMENT POS", C_BLACK, FS_X1, A_CENTER);
    canvas->AddRect(1, 21, 82, 1, C_BLACK);

    if (this->selectedButton == PAVB_POSITION && (!this->jointDrive->isBlink || !this->isInput))
    {
        canvas->AddRect(1, 24, 42, 11, C_BLACK);
        canvas->AddText(3, 26, 78, String(this->GetPort()->filamentPosition), C_WHITE, FS_X1, A_LEFT);
    }
    else
    {
        canvas->AddRect(1, 24, 42, 11, C_BLACK);
        canvas->AddRect(2, 25, 40, 9, C_WHITE);
        canvas->AddText(3, 26, 78, String(this->GetPort()->filamentPosition), C_BLACK, FS_X1, A_LEFT);
    }

    if (this->selectedButton == PAVB_BACK)
        canvas->AddImage(1, 37, &IMG_BACK_BTN_SELECTED);
    else
        canvas->AddImage(1, 37, &IMG_BACK_BTN_UNSELECTED);
}

void PortAdjustView::LeftBtnClick()
{
    if (this->isInput && this->selectedButton == PAVB_POSITION)
        this->jointDrive->AdjustPort(this->portIndex, D_BACKWARD);
    else if (!this->isInput && this->selectedButton > PAVB_BACK)
        this->selectedButton = (PortAdjustViewButton)(this->selectedButton - 1);
}

void PortAdjustView::RightBtnClick()
{
    if (this->isInput && this->selectedButton == PAVB_POSITION)
        this->jointDrive->AdjustPort(this->portIndex, D_FORWARD);
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