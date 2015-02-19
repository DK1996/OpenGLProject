#ifndef QUAT_H_
#define QUAT_H_

#include "Application.h"
#include "Camera.h"

struct KeyFrame
{
	vec3 position;
	quat rotation;
};

class Quaternions : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	float dt;
	float m_timer;

	Camera* m_camera;

	KeyFrame m_hip_Frame[2];
	KeyFrame m_knee_Frame[2];
	KeyFrame m_ankle_Frame[2];

	mat4 m_hip_Bone;
	mat4 m_knee_Bone;
	mat4 m_ankle_Bone;

};

#endif