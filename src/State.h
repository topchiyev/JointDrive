#ifndef STATE_H
#define STATE_H

#include <cstdint>

enum PortStatus
{
    EMPTY = 0,
    LOADING = 1,
    LOADED = 2,
    UNLOADING = 3,
    PUSHING = 4,
    FEEDING = 5,
    PULLING = 6
};

struct PortState
{
    uint16_t index = 0;
    PortStatus status = EMPTY;
    int16_t filamentPosition = 0;
};

struct JointDriveState
{
    bool isInitialized = false;
    uint16_t feedingDistance = 0;
    PortState ports[5];
};

#endif