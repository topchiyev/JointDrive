#include "Storage.h"

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