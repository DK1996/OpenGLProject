#include "SceneManagment.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "AABB.h"
#include "BoundingSphere.h"

bool SceneManagment::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	return true;
}

void SceneManagment::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool SceneManagment::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	m_camera->Update(dt);

	vec4 plane(0, 1, 0, -1);
	
	float d = dot(vec3(plane), vec3(4, 2, 0)) + plane.w;

	if (d < 0) { printf("Back\n"); }
	else if (d > 0) { printf("Front\n"); }
	else { printf("On the plane\n"); }

	return true;
}

void SceneManagment::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}