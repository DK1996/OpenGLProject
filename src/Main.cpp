//#include "IntroToOpenGL.h"
//#include "CameraAndProjections.h"
//#include "RenderingGeometry.h"
//#include "Texturing.h"
//#include "Lighting.h"
//#include "AdvancedTexture.h"
//#include "Quaternions.h"
//#include "Animation.h"
//#include "ParticleSystems.h"
//#include "GPUParticles.h"
#include "SceneManagment.h"

#include <cstdio>

int main()
{
	SceneManagment app;

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