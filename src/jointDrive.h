#ifndef JOINT_DRIVE_H
#define JOINT_DRIVE_H 1

#include "Canvas.h"
#include "State.h"
#include "Storage.h"
#include "Direction.h"
#include "View.h"
#include "MotorController.h"

class JointDrive: public MotorControllerDelegate
{
    public:
        bool isBlink = false;
        explicit JointDrive();
        JointDriveState * GetState();
        Canvas * GetCanvas();
        uint32_t GetFeedingDistance();
        uint32_t GetLoadedDistance();
        void SetFeedingDistance(uint32_t value);
        PortState * GetPort(uint16_t portIndex);
        bool IsPortBusy(uint16_t portIndex, bool includeFeeding = false);
        void Refresh();

        void GoToIntroView();
        void GoToMainView();
        void GoToSettingsView();
        void GoToPortsView(uint16_t portIndex = 1);
        void GoToPortAdjustView(uint16_t portIndex = 1);

        void LoadPort(uint16_t portIndex);
        void UnloadPort(uint16_t portIndex);
        void PushPort(uint16_t portIndex);
        void PullPort(uint16_t portIndex);
        void AdjustPort(uint16_t portIndex, Direction direction);
        void CancelTask();
        void OnMotorControllerStep(uint16_t portIndex, uint32_t distance);
        void OnMotorControllerFinish(uint16_t portIndex);

    private:
        Storage storage;
        JointDriveState state;
        Canvas canvas;
        MotorController motorController;
        View * currentView = nullptr;
        const uint32_t blinkInterval = 100;
        uint32_t blinkChangeTime = 0;
        void Update();
        void Draw();
};

#endif