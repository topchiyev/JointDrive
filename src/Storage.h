#ifndef STORAGE_H
#define STORAGE_H

#include <EEPROM.h>
#include "State.h"

const int JOINT_DRIVE_STORAGE_EEPROM_KEY = 0x00;

class Storage
{
    public:
        explicit Storage();
        JointDriveState GetState();
        void SetState(JointDriveState state);
        void ClearState();
};

#endif