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