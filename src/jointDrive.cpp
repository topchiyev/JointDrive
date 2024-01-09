#include <iostream>
#include "JointDrive.h"

JointDrive::JointDrive()
{
    JointDriveState state = this->storage.GetState();
    if (!state.isInitialized)
    {
        state.feedingDistance = 100;
        state.ports[0].index = 1;
        state.ports[1].index = 2;
        state.ports[2].index = 3;
        state.ports[3].index = 4;
        state.ports[4].index = 5;
        state.isInitialized = true;
        this->storage.SetState(state);
        this->state = state;
    }
}

Canvas * JointDrive::GetCanvas()
{
    return &this->canvas;
}

JointDriveState * JointDrive::GetState()
{
    return &this->state;
}

void JointDrive::Refresh()
{
    this->canvas.Clear();
    // ssd1306_drawBitmap(0, 0, 128, 64, ADJUST_BTN_SELECTED);
}