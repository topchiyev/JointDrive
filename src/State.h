#ifndef STATE_H
#define STATE_H

#include <stdint.h>

enum PortStatus
{
    PS_EMPTY = 0,
    PS_LOADING = 1,
    PS_LOADED = 2,
    PS_UNLOADING = 3,
    PS_PUSHING = 4,
    PS_FEEDING = 5,
    PS_PULLING = 6
};

struct PortState
{
    uint16_t index = 0;
    PortStatus status = PS_EMPTY;
    int16_t filamentPosition = 0;
};

struct JointDriveState
{
    bool isInitialized = false;
    uint16_t feedingDistance = 0;
    PortState ports[5];
};

#endif