#ifndef INTRO_VIEW_H
#define INTRO_VIEW_H

#include "View.h"
#include "JointDrive.h"

const int HIDE_INFO_AFTER = 5000;

class IntroView: public View
{   
    public: 
        ViewType viewType = V_INTRO;
        IntroView(JointDrive * jointDrive);
        void Update(uint32_t time);
        void Draw();

    private:
        JointDrive * jointDrive;
        uint32_t hideTime = 0;
};

#endif