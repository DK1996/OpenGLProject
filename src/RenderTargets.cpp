#include "RenderTargets.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"
#include "PostProcess.h"

bool RenderTargets::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	GenerateFrameBuffer();
	GeneratePlane();
	LoadShader("./Shaders/Texture_Vertex.glsl", 0, "./Shaders/Texture_Fragment.glsl", &m_program_ID);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	return true;
}

void RenderTargets::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool RenderTargets::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	dt = glfwGetTime();
	glfwSetTime(0.0);

	m_camera->Update(dt);

	Gizmos::addSphere(vec3(0, 5, 0), 0.5f, 12, 12, vec4(1, 1, 0, 1));

	return true;
}

void RenderTargets::Draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glViewport(0, 0, 512, 512);
	glClearColor(0.75f, 0.75f, 0.75f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 world = m_camera->m_worldTransform;

	vec4 plane = vec4(0, 0, 1, -5);

	vec3 reflected = reflect(-world[2].xyz(), plane.xyz());
	reflected = normalize(reflected);

	float dist = (glm::dot((plane.xyz() * plane.w) - world[3].xyz(), plane.xyz()) / (glm::dot(plane.xyz(), -world[2].xyz())));

	vec3 inter = world[3].xyz() - world[2].xyz() * dist;

	world[3].xyz = inter - reflected * dist;
	world[2].xyz = -reflected;

	vec3 up = vec3(0, 1, 0);
	world[0].xyz = normalize(cross(world[2].xyz(), up));
	world[1].xyz = normalize(cross(world[0].xyz(), world[2].xyz()));

	mat4 view = inverse(world);
	mat4 proj_View = m_camera->m_projectionView * view;

	glUseProgram(m_program_ID);
	
	int projection_View_Uniform = glGetUniformLocation(m_program_ID, "projection_view");

	glUniformMatrix4fv(projection_View_Uniform, 1, GL_FALSE, (float*)&proj_View);

	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_index_Count, GL_UNSIGNED_INT, 0);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	
	glDepthFunc(GL_GREATER);
	Gizmos::draw(proj_View);
	glDepthFunc(GL_LESS);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 1280, 720);
	glClearColor(0.25f, 0.25f, 0.25f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glUseProgram(m_program_ID);

	int proj_View_Ref_Uniform	= glGetUniformLocation(m_program_ID, "projection_view_reflected");
	int diffuse_Uniform			= glGetUniformLocation(m_program_ID, "diffuse");

	glUniformMatrix4fv(projection_View_Uniform, 1, GL_FALSE, (float*)&m_camera->m_projectionView);
	
	glUniformMatrix4fv(proj_View_Ref_Uniform, 1, GL_FALSE, (float*)&proj_View);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_Texture);

	glUniform1i(diffuse_Uniform, 0);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_index_Count, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void RenderTargets::GenerateFrameBuffer()
{
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

	glGenTextures(1, &m_fbo_Texture);
	glBindTexture(GL_TEXTURE_2D, m_fbo_Texture);
	glTexStorage2D(GL_TEXTURE_2D, 10, GL_RGBA8, 512, 512);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_fbo_Texture, 0);

	glGenRenderbuffers(1, &m_fbo_Depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_Depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_fbo_Depth);

	GLenum draw_Buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, draw_Buffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer Error!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTargets::GeneratePlane()
{
	float vertex_Data[] =
	{
		// Position,		UV.
		-5,  0, -5, 1,		0, 0,
		 5,  0, -5, 1,		1, 0,
		 5, 10, -5, 1,		1, 1,
		-5, 10, -5, 1,		0, 1,
	};

	unsigned int index_Data[] =
	{
		0, 1, 2,
		0, 2, 3,
	};

	m_plane.m_index_Count = 6;

	glGenVertexArrays(1, &m_plane.m_VAO);
	glBindVertexArray(m_plane.m_VAO);

	glGenBuffers(1, &m_plane.m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 6 * 4, vertex_Data, GL_STATIC_DRAW);

	glGenBuffers(1, &m_plane.m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_Data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, (void*)(sizeof(float)* 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}