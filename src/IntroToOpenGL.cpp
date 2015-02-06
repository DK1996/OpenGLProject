#include "IntroToOpenGL.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Gizmos.h"

bool IntroToOpenGL::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}
	
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	m_view = glm::lookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_projection = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);

	m_camera_x = -10.0f;
	m_camera_z = -10.0f;

	m_timer = 0.0f;

	return true;
}

void IntroToOpenGL::ShutDown()
{
	Gizmos::destroy();
}

bool IntroToOpenGL::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	// This is where our cool code will be.

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_timer += dt;
	//camera_x = sinf(timer) * 10;
	//camera_z = cosf(timer) * 10;

	
	//camera_x += dt;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::clear();

	Gizmos::addTransform(mat4(1), 10);

	mat4 sun_transform = glm::rotate(m_timer, vec3(0, 1, 0));
	mat4 planet_transform1 = sun_transform * glm::translate(vec3(5, 0, 0)) * glm::rotate(m_timer, vec3(0, 1, 0));
	mat4 moon_transform1 = planet_transform1 * glm::translate(vec3(1, 0, 0));

	//Gizmos::addSphere(sun_transform[3].xyz, 1.0f, 20, 20, orange, &sun_transform);
	//Gizmos::addSphere(planet_transform1[3].xyz, 0.5f, 20, 20, blue, &planet_transform1);
	//Gizmos::addSphere(moon_transform1[3].xyz, 0.25f, 20, 20, black, &moon_transform1);

	//Gizmos::addTri(vec3(0, 1, 0), vec3(2, -1, 1), vec3(-3, -2, 4), red);

	//Gizmos::addAABB(vec3(2.5f, 5, 0), vec3(1, 1, 1), blue);

	//Gizmos::addAABBFilled(vec3(-2.5f, 5, 0), vec3(1, 1, 1), blue);

	

	return true;
}

void IntroToOpenGL::Draw()
{
	mat4 view = glm::lookAt(vec3(m_camera_x, 5, m_camera_z), vec3(0, 0, 0), vec3(0, 1, 0));
	Gizmos::draw(m_projection, view);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}