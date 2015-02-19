#include "Quaternions.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>

bool Quaternions::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	quat my_Quaternion(1, 0, 0, 0);
	quat euler_Quat(vec3(3, 5, 7));

	quat mixed_Quat = slerp(my_Quaternion, euler_Quat, 0.8f);
	
	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);
	
	float PI = 3.14159;

	m_hip_Frame[0].position = vec3(0, 5, 0);
	m_hip_Frame[0].rotation = quat(vec3(-1, 0, 0));

	m_knee_Frame[0].position = vec3(0, -2.5, 0);
	m_knee_Frame[0].rotation = quat(vec3(-1, 0, 0));

	m_ankle_Frame[0].position = vec3(0, -2.5, 0);
	m_ankle_Frame[0].rotation = quat(vec3(1, 0, 0));

	m_hip_Frame[1].position = vec3(0, 5, 0);
	m_hip_Frame[1].rotation = quat(vec3(1, 0, 0));

	m_knee_Frame[1].position = vec3(0, -2.5, 0);
	m_knee_Frame[1].rotation = quat(vec3(0, 0, 0));

	m_ankle_Frame[1].position = vec3(0, -2.5, 0);
	m_ankle_Frame[1].rotation = quat(vec3(1, 0, 0));

	return true;
}

mat4 EvaluateKeyFrames(KeyFrame _start, KeyFrame _end, float _t)
{
	vec3 pos = mix(_start.position, _end.position, _t);
	quat rot = slerp(_start.rotation, _end.rotation, _t);

	mat4 result = translate(pos) * toMat4(rot);

	return result;
}

void Quaternions::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool Quaternions::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}
	
	m_camera->Update(dt);

	dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_timer = dt;

	float sin_Wave = sinf(m_timer) * 0.5f + 0.5f;

	m_hip_Bone = EvaluateKeyFrames(m_hip_Frame[0], m_hip_Frame[1], sin_Wave);
	m_knee_Bone = EvaluateKeyFrames(m_knee_Frame[0], m_knee_Frame[1], sin_Wave);
	m_ankle_Bone = EvaluateKeyFrames(m_ankle_Frame[0], m_ankle_Frame[1], sin_Wave);
	
	mat4 global_Hip = m_hip_Bone;
	mat4 global_Knee = m_hip_Bone * m_knee_Bone;
	mat4 global_Ankle = m_knee_Bone * m_ankle_Bone;

	vec3 hip_Pos = global_Hip[3].xyz;
	vec3 knee_Pos = global_Knee[3].xyz;
	vec3 ankle_Pos = global_Ankle[3].xyz;

	Gizmos::addAABBFilled(hip_Pos, vec3(0.5f), vec4(1, 1, 0, 1), &global_Hip);
	Gizmos::addAABBFilled(knee_Pos, vec3(0.5f), vec4(1, 0, 1, 1), &global_Knee);
	Gizmos::addAABBFilled(ankle_Pos, vec3(0.5f), vec4(0, 1, 1, 1), &global_Ankle);

	Gizmos::addLine(hip_Pos, knee_Pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));
	Gizmos::addLine(knee_Pos, ankle_Pos, vec4(0, 1, 0, 1), vec4(1, 0, 0, 1));

	return true;
}

void Quaternions::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}