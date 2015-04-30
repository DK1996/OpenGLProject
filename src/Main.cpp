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
//#include "SceneManagment.h"
//#include "Threading.h"
//#include "RenderTargets.h"
//#include "PostProcess.h"
//#include "Shadows.h"
//#include "DeferredRendering.h"
//#include "ProceduralGeneration.h"
//#include "AdvancedNavigation.h"

//#include "Assignment.h"

#include "GameTreeSearch.h"

#include <cstdio>

int main()
{
	GameTreeSearch app;

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