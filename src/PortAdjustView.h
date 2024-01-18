#ifndef PORT_ADJUST_VIEW_H
#define PORT_ADJUST_VIEW_H

#include "JointDrive.h"
#include "State.h"

enum PortAdjustViewButton
{
    PAVB_BACK = 0,
    PAVB_POSITION = 1,
};

class PortAdjustView
{
    public:
        void Begin(JointDrive * jointDrive, uint16_t portIndex);
        void Draw(Canvas * canvas);
        void LeftBtnClick();
        void RightBtnClick();
        void ActionBtnClick();

    private:
        JointDrive * jointDrive;
        PortAdjustViewButton selectedButton = PAVB_BACK;
        uint8_t portIndex = 1;
        bool isInput = false;
        PortState * GetPort();
};

#endif