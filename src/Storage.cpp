#include "Storage.h"

void Storage::Begin()
{
    EEPROM.begin();
}

JointDriveState Storage::GetState()
{
    JointDriveState state;
    EEPROM.get(JOINT_DRIVE_STORAGE_EEPROM_KEY, state);

    return state;
}

void Storage::SetState(JointDriveState state)
{
    EEPROM.put(JOINT_DRIVE_STORAGE_EEPROM_KEY, state);
}

void Storage::ClearState()
{
    for (int i = JOINT_DRIVE_STORAGE_EEPROM_KEY ; i < JOINT_DRIVE_STORAGE_EEPROM_KEY + sizeof(JointDriveState); i++) {
        EEPROM.write(i, 0);
    }
}