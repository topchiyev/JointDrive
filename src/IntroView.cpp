#include "IntroView.h"
#include "Images.h"
#include "Color.h"
#include "Align.h"
#include "FontSize.h"
#include "Version.h"

// WIDTH = 84
// HEIGHT = 48

void IntroView::Begin(JointDrive *jointDrive)
{
    this->jointDrive = jointDrive;
    uint32_t curTime = millis();
    this->hideTime = curTime + HIDE_INFO_AFTER;
};

void IntroView::Update(uint32_t time)
{
    //if (time >= this->hideTime)
        //this->jointDrive->GoToMainView();
}

void IntroView::Draw(Canvas * canvas)
{
    canvas->AddImage(1, 4, &IMG_INTRO);
    canvas->AddText(1, 40, 82, "VERSION  " + String(JOINT_DRIVE_VERSION), C_BLACK, FS_X1, A_CENTER);
};