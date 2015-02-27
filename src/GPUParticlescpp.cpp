#include "GPUParticles.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"

bool GPUParticles::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	m_time = 0;

	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	LoadShader("./Shaders/GPU_Particle_Vert.glsl", "./Shaders/GPU_Particle_Geom.glsl", "./Shaders/GPU_Particle_Frag.glsl", &m_programID);
	
	m_emitter.Init(1000, vec3(0, 0, 0), 10, 0.5f, 1, 2.5f, 5, 0.5f, 2.5f, vec4(1, 0, 0, 1), vec4(1, 0.4f, 0, 1));

	return true;
}

void GPUParticles::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool GPUParticles::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	m_dt = glfwGetTime();
	glfwSetTime(0.0);

	m_camera->Update(m_dt);

	m_time += m_dt;

	m_emitter.CreateBuffers();
	m_emitter.CreateUpdateShader();

	return true;
}

void GPUParticles::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);
	
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}