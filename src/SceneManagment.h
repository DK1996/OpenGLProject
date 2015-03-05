#ifndef SCENE_MANAGMENT_H
#define SCENE_MANAGMENT_H

#include "Application.h"
#include "Camera.h"

class SceneManagment : public Application
{
public:

	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

private:

	BoundingSphere m_sphere;

	Camera* m_camera;

	float dt;

};

#endif // !SCENE_MANAGMENT_H
