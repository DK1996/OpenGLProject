#include "PostProcess.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"

bool PostProcess::StartUp()
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

	//Create the framebuffer.
	BuildFrameBuffer();

	// Create the quad mesh.
	BuildQuad();

	// Load the post effect shader.
	LoadShader("./Shaders/Post_Vertex.glsl", 0, "./Shaders/Post_Fragment.glsl", &m_post_Program);

	return true;
}

void PostProcess::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool PostProcess::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	dt = glfwGetTime();
	glfwSetTime(0.0);
	m_camera->Update(dt);

	Gizmos::addSphere(vec3(0, 10, 0), 0.5f, 15, 15, vec4(1, 0.6f, 0, 1));

	return true;
}

void PostProcess::Draw()
{
	// Bind framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, 1280, 720);

	// Clear screen.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render everything like normal, but to the FBO_Texture.
	Gizmos::draw(m_camera->m_projectionView);

	// Bind backbuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind post effect shader.
	glUseProgram(m_post_Program);

	// Bind the FBO_Texture.
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FBO_Texture);

	int tex_Location = glGetUniformLocation(m_post_Program, "input_Texture");
	glUniform1i(tex_Location, 0);

	// Render our quad with the texture on it.
	glBindVertexArray(m_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void PostProcess::BuildFrameBuffer()
{
	glGenFramebuffers(1, &m_FBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_FBO_Texture);
	glBindTexture(GL_TEXTURE_2D, m_FBO_Texture);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_FBO_Texture, 0);

	glGenRenderbuffers(1, &m_FBO_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_FBO_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1280, 720);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER_RED_SIZE, m_FBO_depth);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	// Error checking.
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Frame buffer not working!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::BuildQuad()
{
	vec2 half_Texel = 1.0f / vec2(1280, 720);

	float vertex_Data[] =
	{
		-1, -1, 0, 1,	half_Texel.x,			half_Texel.y,
		-1,  1, 0, 1,	half_Texel.x,			1.0f - half_Texel.y,
		 1,  1, 0, 1,	1.0f - half_Texel.x,	1.0f - half_Texel.y,
		 1, -1, 0, 1,	1.0f - half_Texel.x,	half_Texel.y
	};

	unsigned int index_Data[] =
	{
		0, 1, 2, 0, 2, 3
	};

	glGenVertexArrays(1, &m_quad.m_VAO);
	glBindVertexArray(m_quad.m_VAO);

	glGenBuffers(1, &m_quad.m_VBO);
	glGenBuffers(1, &m_quad.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_Data), vertex_Data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_Data), index_Data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Tex Coord.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 6, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float)* 6, (void*)(sizeof(float)* 4));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}