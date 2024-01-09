#ifndef JOINT_DRIVE_H
#define JOINT_DRIVE_H 1

#include "Canvas.h"
#include "State.h"
#include "Storage.h"
#include "Images.h"

class JointDrive
{
    public:
        explicit JointDrive();
        void Refresh();
        Canvas * GetCanvas();
        JointDriveState * GetState();

    private:
        Storage storage;
        JointDriveState state;
        Canvas canvas;
};

#endif