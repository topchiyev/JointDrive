#ifndef PORTS_VIEW
#define PORTS_VIEW

#include "JointDrive.h"

enum PortsViewButton
{
    PVB_NONE = -1,
    PVB_BACK = 0,
    PVB_PORT = 1,
    PVB_LOAD = 5,
    PVB_UNLOAD = 6,
    PVB_FEED = 7,
    PVB_PULL = 8,
    PVB_CANCEL = 9,
    PVB_ADJUST = 10,
    PVB_READY = 11,
};

class PortsView
{
    public:
        void Begin(JointDrive * jointDrive, uint16_t portIndex = 1);
        void Draw(Canvas * canvas);
        void LeftBtnClick();
        void RightBtnClick();
        void ActionBtnClick();
        void PortChanged();
        void SwitchingFinished();

    private:
        JointDrive * jointDrive;
        PortsViewButton selectedButton = PVB_BACK;
        uint16_t selectedPortIndex = 1;
        bool isInput = false;
        PortsViewButton * activeButtons;
        uint16_t activeButtonsLen;
        PortState * GetPort();
        bool IsButtonActive(PortsViewButton btn);
        PortsViewButton GetPrevActiveButton(PortsViewButton button);
        PortsViewButton GetNextActiveButton(PortsViewButton button);
};

#endif