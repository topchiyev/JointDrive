#ifndef JOINT_DRIVE_H
#define JOINT_DRIVE_H 1

#include "Canvas.h"
#include "State.h"
#include "MotorController.h"
#include "RotaryEncoderController.h"

class JointDrive: public MotorControllerDelegate, public RotaryEncoderControllerDelegate
{
    public:
        bool isBlink = false;
        bool isHoming = false;
        void Begin();
        JointDriveState * GetState();
        Canvas * GetCanvas();
        uint32_t GetFeedingDistance();
        uint32_t GetLoadedDistance();
        void SetFeedingDistance(uint32_t value);
        PortState * GetPort(uint16_t portIndex);
        bool IsPortBusy(uint16_t portIndex, bool includeFeeding = false);
        uint32_t GetSgResult();
        void Refresh();

        void GoToIntroView();
        void IntroViewFinished();
        void GoToMainView();
        void GoToSettingsView();
        void GoToPortsView(uint16_t portIndex = 1);
        void GoToPortAdjustView(uint16_t portIndex = 1);

        void LoadPort(uint16_t portIndex);
        void UnloadPort(uint16_t portIndex);
        void PushPort(uint16_t portIndex);
        void PullPort(uint16_t portIndex);
        void AdjustPort(uint16_t portIndex, Direction direction);
        void MakePortAdjustmentMove();
        void CancelTask();

        void OnMotorControllerFinishedHoming();
        void OnMotorControllerFinishedSwitching(SwitchMotorPosition position);
        void OnMotorControllerMoved(uint16_t portIndex, uint32_t distanceLeft);
        void OnMotorControllerFinishedMoving(uint16_t portIndex);

        void OnRotaryEncoderIncreased();
        void OnRotaryEncoderDecreased();
        void OnRotaryEncoderPressed();

    private:
        void Update();
        void UpdateActions();
        void Draw();
};

#endif