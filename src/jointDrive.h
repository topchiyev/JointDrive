#ifndef JOINT_DRIVE_H
#define JOINT_DRIVE_H

#include "lcdgfx.h"

class JointDrive
{
    private:
        DisplaySSD1306_128x64_I2C canvas;

    public:
        JointDrive();
        void Refresh() const;
};

#endif