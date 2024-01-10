#ifndef VIEW_H
#define VIEW_H

#include <stdint.h>

enum ViewType
{
    V_NONE = 0,
    V_INTRO = 1,
    V_MAIN = 2,
    V_SETTINGS = 3,
    V_PORTS = 4,
    V_PORT_ADJUST = 5,
};

class View
{
    public:
        ViewType viewType = V_NONE;
        void Update(uint32_t time) { }
        void Draw() { }
        void LeftBtnClick() { }
        void RightBtnClick() { }
        void ActionBtnClick() { }
        void PortChanged() { }
};

#endif
