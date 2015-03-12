#ifndef THREADING_H
#define THREADING_H

#include "Application.h"
#include "Camera.h"

#include <thread>

class Threading : public Application
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

#endif // !THREADING_H