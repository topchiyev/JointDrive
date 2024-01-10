#ifndef PORTS_VIEW
#define PORTS_VIEW

#include "View.h"
#include "JointDrive.h"

enum PortsViewButton
{
    PVB_BACK = 0,
    PVB_PORT = 1,
    PVB_LOAD = 5,
    PVB_UNLOAD = 6,
    PVB_FEED = 7,
    PVB_PULL = 8,
    PVB_CANCEL = 9,
    PVB_ADJUST = 10,
};

class PortsView: public View
{
    public:
        PortsView(JointDrive * jointDrive, uint16_t portIndex = 1);
        ViewType viewType = V_PORTS;
        void Draw();
        void LeftBtnClick();
        void RightBtnClick();
        void ActionBtnClick();

    private:
        JointDrive * jointDrive;
        PortsViewButton selectedButton = PVB_BACK;
        uint16_t selectedPortIndex = 1;
        bool isInput = false;
        PortsViewButton * activeButtons;
        uint16_t activeButtonsLen;
        void PortChanged();
        PortState * GetPort();
        bool IsButtonActive(PortsViewButton btn);
};

#endif