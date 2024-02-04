#include "SettingsView.h"
#include "img/img-back-btn-selected.h"
#include "img/img-back-btn-unselected.h"
#include "img/img-free-btn-selected.h"
#include "img/img-free-btn-unselected.h"
#include "img/img-reset-btn-selected.h"
#include "img/img-reset-btn-unselected.h"

void SettingsView::Begin(JointDrive * jointDrive)
{
    this->jointDrive = jointDrive;
    this->distance = jointDrive->GetFeedingDistance();
}

void SettingsView::Draw(Canvas * canvas)
{
    canvas->AddText(1, 1, 82, "DISTANCE TO", C_BLACK, FS_X1, A_CENTER);
    canvas->AddText(1, 9, 82, "EXTRUDER (MM)", C_BLACK, FS_X1, A_CENTER);
    canvas->AddRect(1, 18, 82, 1, C_BLACK);
    
    if (this->selectedButton == SVB_DISTANCE && (!this->jointDrive->isBlink || !this->isInput))
    {
        canvas->AddRect(1, 22, 42, 11, C_BLACK);
        canvas->AddText(3, 24, 78, String(this->distance), C_WHITE, FS_X1, A_LEFT);
    }
    else
    {
        canvas->AddRect(1, 22, 42, 11, C_BLACK);
        canvas->AddRect(2, 23, 40, 9, C_WHITE);
        canvas->AddText(3, 24, 78, String(this->distance), C_BLACK, FS_X1, A_LEFT);
    }

    if (this->selectedButton == SVB_BACK)
        canvas->AddImage(1, 37, &IMG_BACK_BTN_SELECTED);
    else
        canvas->AddImage(1, 37, &IMG_BACK_BTN_UNSELECTED);
        
    if (this->selectedButton == SVB_FREE)
        canvas->AddImage(26, 37, &IMG_FREE_BTN_SELECTED);
    else
        canvas->AddImage(26, 37, &IMG_FREE_BTN_UNSELECTED);
    
    if (this->selectedButton == SVB_RESET)
        canvas->AddImage(51, 37, &IMG_RESET_BTN_SELECTED);
    else
        canvas->AddImage(51, 37, &IMG_RESET_BTN_UNSELECTED);
}

void SettingsView::LeftBtnClick()
{
    if (this->isInput && this->selectedButton == SVB_DISTANCE && this->distance > 0)
    {
        this->distance = this->distance - 1;
    }
    else if (!this->isInput && this->selectedButton > SVB_BACK)
    {
        this->selectedButton = (SettingsViewButton)(this->selectedButton - 1);
    }
}

void SettingsView::RightBtnClick()
{
    if (this->isInput && this->selectedButton == SVB_DISTANCE and this->distance < 5000)
    {
        this->distance = this->distance + 1;
    }
    else if (!this->isInput && this->selectedButton < SVB_RESET)
    {
        this->selectedButton = (SettingsViewButton)(this->selectedButton + 1);
    }
}

void SettingsView::ActionBtnClick()
{
    if (this->selectedButton == SVB_DISTANCE)
    {
        if (this->isInput)
            this->isInput = false;
        else
            this->isInput = true;
    }       
    else if (this->selectedButton == SVB_BACK)
    {
        this->jointDrive->SetFeedingDistance(this->distance);
        this->jointDrive->GoToMainView();
    }
    else if (this->selectedButton == SVB_FREE)
    {
        this->jointDrive->SwitchToFreePosition();
    }
    else if (this->selectedButton == SVB_RESET)
    {
        this->jointDrive->ResetState();
    }
}
