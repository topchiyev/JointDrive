#include "IntroView.h"
#include "Images.h"
#include "Color.h"
#include "Align.h"
#include "FontSize.h"
#include "Version.h"

IntroView::IntroView(JointDrive *jointDrive)
{
    this->jointDrive = jointDrive;
    uint32_t curTime = millis();
    this->hideTime = curTime + HIDE_INFO_AFTER;
};

void IntroView::Update(uint32_t time)
{
    if (time >= this->hideTime)
        this->jointDrive->GoToMainView();
}

void IntroView::Draw()
{
    Canvas * canvas = this->jointDrive->GetCanvas();

    canvas->AddImage(0, 0, &IMG_INTRO);
    canvas->AddText(69, 52, 53, "VERSION", C_WHITE, FS_X1, A_LEFT);
    canvas->AddText(69, 52, 53, JOINT_DRIVE_VERSION, C_WHITE, FS_X1, A_RIGHT);
};