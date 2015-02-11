#include "IntroToOpenGL.h"
#include "CameraAndProjections.h"
#include "RenderingGeometry.h"

#include <cstdio>

int main()
{
	RenderingGeometry app;

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