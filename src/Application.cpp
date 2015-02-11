#include "Application.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"

#include <cstdio>

Application::Application() {}

Application::~Application() {}

bool Application::StartUp()
{
	if (glfwInit() == false)
	{
		return false;
	}

	this->m_window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	if (this->m_window == nullptr)
	{
		return false;
	}

	glfwMakeContextCurrent(this->m_window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(this->m_window);
		glfwTerminate();

		return false;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();

	printf("Successfully loaded OpenGL version %d.%d\n", major_version, minor_version);

	return true;
}

void Application::ShutDown()
{
	glfwDestroyWindow(this->m_window);
	glfwTerminate();
}

bool Application::Update()
{
	if (glfwWindowShouldClose(this->m_window))
	{
		return false;
	}
	
	float dt = glfwGetTime();
	glfwSetTime(0.0);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	vec4 green(0, 1, 0, 1);
	vec4 red(1, 0, 0, 1);
	vec4 blue(0, 0, 1, 1);
	vec4 orange(1, 0.5f, 0, 1);
	
	for (int i = 0; i <= 101; ++i)
	{
		Gizmos::addLine(vec3(-50 + i, 0, -50), vec3(-50 + i, 0, 50), i % 10 == 0 ? white : black);
		Gizmos::addLine(vec3(-50, 0, -50 + i), vec3(50, 0, -50 + i), i % 10 == 0 ? white : black);
	}

	return true;
}

void Application::Draw() {}