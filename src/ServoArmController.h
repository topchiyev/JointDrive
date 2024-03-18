#ifndef SERVO_ARM_CONTROLLER_H
#define SERVO_ARM_CONTROLLER_H

class ServoArmController
{
    public:
        void Begin();
        bool IsReady();
        void SetReady();
        void SetFree();
    private:
        bool isReady = false;
};

#endif