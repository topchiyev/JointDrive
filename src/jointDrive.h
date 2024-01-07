#ifndef JOINT_DRIVE_H
#define JOINT_DRIVE_H

class JointDrive
{
    private:
        int canvas {};
        int m_month {};
        int m_day {};

    public:
        void Refresh() const;
};

#endif