#include "ParticleSystems.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"

bool ParticleSystems::StartUp()
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

	LoadShader("./Shaders/Particle_Vertex.glsl", "./Shaders/Particle_Fragment.glsl", &m_program_ID);

	m_emitter.Init(10000, vec3(0, 0, 0), 500, 1, 5, 5.f, 25.f, 0.25f, 0.25f, vec4(1, 1, 1, 1), vec4(0, 0, 0, 1));

	return true;
}

void ParticleSystems::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool ParticleSystems::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	m_dt = glfwGetTime();
	glfwSetTime(0.0);
	
	m_camera->Update(m_dt);

	m_emitter.Update(m_dt);
	m_emitter.UpdateVertexData(m_camera->m_worldTransform[3].xyz, m_camera->m_worldTransform[1].xyz);

	return true;
}

void ParticleSystems::Draw()
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glUseProgram(m_program_ID);
	
	int proj_View_Uniform = glGetUniformLocation(m_program_ID, "projection_view");

	glUniformMatrix4fv(proj_View_Uniform, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	m_emitter.Render();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}