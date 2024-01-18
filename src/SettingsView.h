#ifndef SETTINGS_VIEW_H
#define SETTINGS_VIEW_H

#include "JointDrive.h"

enum SettingsViewButton
{
    SVB_BACK = 0,
    SVB_DISTANCE = 1,
    SVB_RESET = 2,
    SVB_SAVE = 3,
};

class SettingsView
{
    public:
        void Begin(JointDrive * jointDrive);
        void Draw(Canvas * canvas);
        void LeftBtnClick();
        void RightBtnClick();
        void ActionBtnClick();

    private:
        JointDrive * jointDrive;
        SettingsViewButton selectedButton = SVB_BACK;
        uint16_t distance = 0;
        bool isInput = false;

        String GetActivity();
        const Image * GetPortIcon(PortStatus status, bool isSelected);
        const Image * GetPortIndexIcon(uint16_t index, bool isSelected);
        
};

#endif
