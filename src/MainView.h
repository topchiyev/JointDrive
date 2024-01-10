#ifndef MAIN_VIEW_H
#define MAIN_VIEW_H

#include "View.h"
#include "JointDrive.h"

enum MainViewButton
{
    MVB_PORTS = 0,
    MVB_SETTINGS = 1,
};

class MainView: public View
{
    public:
        MainView(JointDrive * jointDrive);
        ViewType viewType = V_MAIN;
        void Draw();
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
