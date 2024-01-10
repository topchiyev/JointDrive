#ifndef PORT_ADJUST_VIEW_H
#define PORT_ADJUST_VIEW_H

#include "View.h"
#include "JointDrive.h"

enum PortAdjustViewButton
{
    PAVB_BACK = 0,
    PAVB_POSITION = 1,
};

class PortAdjustView: public View
{
    public:
        PortAdjustView(JointDrive * jointDrive, uint16_t portIndex);
        ViewType viewType = V_PORT_ADJUST;
        void Draw();
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