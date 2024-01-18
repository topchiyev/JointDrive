#include "JointDrive.h"

JointDrive jointDrive;

void setup()
{
	jointDrive.Begin();
}

void loop()
{
	jointDrive.Refresh();
}