#include "IntroView.h"
#include "Images.h"
#include "Color.h"
#include "Align.h"
#include "FontSize.h"
#include "Version.h"

IntroView::IntroView(JointDrive *jointDrive)
{
    this->jointDrive = jointDrive;
};

void IntroView::Draw()
{
    Canvas * canvas = this->jointDrive->GetCanvas();

    canvas->AddImage(0, 0, &INTRO);
    canvas->AddText(69, 52, 53, "VERSION", WHITE, X1, REGULAR, LEFT);
    canvas->AddText(69, 52, 53, JOINT_DRIVE_VERSION, WHITE, X1, REGULAR, RIGHT);
};