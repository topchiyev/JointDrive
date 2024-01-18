#include "SettingsView.h"
#include "Images.h"

void SettingsView::Begin(JointDrive * jointDrive)
{
    this->jointDrive = jointDrive;
    this->distance = jointDrive->GetFeedingDistance();
}

void SettingsView::Draw(Canvas * canvas)
{
    canvas->AddText(2, -3, 128-4, "SETTINGS", C_WHITE, FS_X2, A_CENTER);
    canvas->AddRect(1, 13, 128-1, 14, C_WHITE);
    canvas->AddText(2, 19, 128-4, "DISTANCE TO EXTRUDER", C_WHITE, FS_X2, A_LEFT);
    
    if (this->selectedButton == SVB_DISTANCE && (!this->jointDrive->isBlink || !this->isInput))
    {
        canvas->AddRect(2, 32, 2 + 40, 33 + 12, C_WHITE);
        canvas->AddText(3, 29, 36, "" + this->distance, C_BLACK, FS_X2, A_LEFT);
    }
    else
    {
        canvas->AddText(3, 29, 36, "" + this->distance, C_WHITE, FS_X2, A_LEFT);
        canvas->AddRect(2, 45, 2 + 40, 45+1, C_WHITE);
    }

    if (this->selectedButton == SVB_BACK)
        canvas->AddImage(2, 52, &IMG_BACK_BTN_SELECTED);
    else
        canvas->AddImage(2, 52, &IMG_BACK_BTN_UNSELECTED);
        
    if (this->selectedButton == SVB_RESET)
        canvas->AddImage(74, 52, &IMG_RESET_BTN_SELECTED);
    else
        canvas->AddImage(74, 52, &IMG_RESET_BTN_UNSELECTED);
        
    if (this->selectedButton == SVB_SAVE)
        canvas->AddImage(104, 52, &IMG_SAVE_BTN_SELECTED);
    else
        canvas->AddImage(104, 52, &IMG_SAVE_BTN_UNSELECTED);
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
    else if (!this->isInput && this->selectedButton < SVB_SAVE)
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
            this->isInput = false;
    }       
    else if (this->selectedButton == SVB_BACK)
    {
        this->jointDrive->GoToMainView();
    }
    else if (this->selectedButton == SVB_RESET)
    {
        this->distance = this->jointDrive->GetFeedingDistance();
    }
    else if (this->selectedButton == SVB_SAVE)
    {
        this->jointDrive->SetFeedingDistance(this->distance);
    }
}
