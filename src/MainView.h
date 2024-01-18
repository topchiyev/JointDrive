#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include "JointDrive.h"

enum MainViewButton
{
    MVB_PORTS = 0,
    MVB_SETTINGS = 1,
};

class MainView
{
    public:
        void Begin(JointDrive * jointDrive);
        void Draw(Canvas * canvas);
        void LeftBtnClick();
        void RightBtnClick();
        void ActionBtnClick();

    private:
        JointDrive * jointDrive;
        MainViewButton selectedButton;

        String GetActivity();
        const Image * GetPortIcon(PortStatus status, bool isSelected);
        const Image * GetPortIndexIcon(uint16_t index, bool isSelected);
        
};

#endif
