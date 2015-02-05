#include "gl_core_4_4.h"
#include "Gizmos.h"

#include <GLFW\glfw3.h>
#include <cstdio>

#define GLM_SWIZZLE

#include "glm/glm.hpp"
#include "glm/ext.hpp"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

int main()
{
	if (glfwInit() == false)
	{
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	if (window == nullptr)
	{
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		
		return -3;
	}

	int major_version = ogl_GetMajorVersion();
	int minor_version = ogl_GetMinorVersion();

	printf("Successfully loaded OpenGL version %d.%d\n", major_version, minor_version);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	mat4 view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	mat4 projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	float camera_x = -10.0f;
	float camera_z = -10.0f;

	float timer = 0.0f;

	glfwSetTime(0.0);
	while (glfwWindowShouldClose(window) == false)
	{
		// This is where our cool code will be.

		float dt = (float)glfwGetTime();
		glfwSetTime(0.0);

		timer += dt;
		//camera_x = sinf(timer) * 10;
		//camera_z = cosf(timer) * 10;

		mat4 view = glm::lookAt(vec3(camera_x, 10, camera_z), vec3(0, 0, 0), vec3(0, 1, 0));
		//camera_x += dt;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Gizmos::clear();

		Gizmos::addTransform( mat4(1), 10 );

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

		mat4 sun_transform = glm::rotate(timer, vec3(0, 1, 0));

		mat4 planet_transform = sun_transform * glm::translate(vec3(5, 0, 0)) * glm::rotate(timer * 2.0f, vec3(0, 1, 0));
		mat4 moon_transform = planet_transform * glm::translate(vec3(1, 0, 0));

		Gizmos::addSphere(sun_transform[3].xyz, 1.0f, 20, 20, orange, &sun_transform);
		Gizmos::addSphere(planet_transform[3].xyz, 0.5f, 20, 20, blue, &planet_transform);
		Gizmos::addSphere(moon_transform[3].xyz, 0.25f, 20, 20, black, &moon_transform);

		//Gizmos::addTri(vec3(0, 1, 0), vec3(2, -1, 1), vec3(-3, -2, 4), red);

		//Gizmos::addAABB(vec3(2.5f, 5, 0), vec3(1, 1, 1), blue);

		//Gizmos::addAABBFilled(vec3(-2.5f, 5, 0), vec3(1, 1, 1), blue);

		Gizmos::draw(projection, view);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Gizmos::destroy();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}