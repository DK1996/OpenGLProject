#include "CameraAndProjections.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>

bool CamerasAndProjections::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	
	Gizmos::create();

	m_camera = new FlyCamera();
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	return true;
}

void CamerasAndProjections::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool CamerasAndProjections::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	dt = glfwGetTime();
	glfwSetTime(0.0);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 orange(1, 0.5f, 0, 1);

	for (int i = 0; i <= 10; ++i)
	{
		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10), i % 10 == 0 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i), i % 10 == 0 ? white : black);
	}

	return true;
}

void CamerasAndProjections::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_camera->Update(dt);
	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}