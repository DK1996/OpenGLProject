#include "IntroToOpenGL.h"
#include "CameraAndProjections.h"

#include <cstdio>

int main()
{
	CamerasAndProjections app;

	if (app.StartUp() == false)
	{
		return -1;
	}
	
	while (app.Update() == true)
	{
		app.Draw();
	}

	app.ShutDown();
	
	return 0;
}