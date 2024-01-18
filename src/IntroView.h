#ifndef INTRO_VIEW_H
#define INTRO_VIEW_H

#include "JointDrive.h"

const int HIDE_INFO_AFTER = 5000;

class IntroView
{   
    public: 
        void Begin(JointDrive * jointDrive);
        void Update(uint32_t time);
        void Draw(Canvas * canvas);

    private:
        JointDrive * jointDrive;
        uint32_t hideTime = 0;
};

#endif