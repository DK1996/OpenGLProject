#ifndef SCENE_MANAGMENT_H
#define SCENE_MANAGMENT_H

#include "Application.h"
#include "Camera.h"
#include "BoundingSphere.h"

class SceneManagment : public Application
{
public:

	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void GetFrustumPlanes(const mat4& _transform, vec4* _planes);

private:

	BoundingSphere m_sphere;

	Camera* m_camera;

	float dt;

};

#endif // !SCENE_MANAGMENT_H
