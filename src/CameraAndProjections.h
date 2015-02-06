#ifndef CAM_PROJ_H
#define CAM_PROJ_H

#include "Application.h"
#include "Camera.h"

class CamerasAndProjections : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

private:

	Camera* m_camera;
	
	float dt;

};

#endif