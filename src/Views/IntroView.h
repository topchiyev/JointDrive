#ifndef INTRO_VIEW_H
#define INTRO_VIEW_H

#include "View.h"
#include "JointDrive.h"

class IntroView: public View
{   
    public: 
        JointDrive * jointDrive;

        IntroView(JointDrive * jointDrive);

        void Draw();
        void LeftBtnClick();
        void RightBtnClick();
        void ActionBtnClick();
};

    // def __init__(self, jointDrive):
    //     self.jointDrive = jointDrive
    //     pass

    // def Update(self):
    //     pass
    
    // def Draw(self, canvas):
    //     canvas.AddImage(0, 0, "Images/intro.png")
    //     canvas.AddText(69, 52, 53, 5, "VERSION", Colors.WHITE, 10, TextAlign.LEFT)
    //     canvas.AddText(69, 52, 53, 5, self.jointDrive.GetVersion(), Colors.WHITE, 10, TextAlign.RIGHT)
    //     pass
    
    // def LeftBtnClick(self):
    //     pass
    
    // def RightBtnClick(self):
    //     pass
    
    // def ActionBtnClick(self):
    //     pass
    

#endif