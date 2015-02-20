#include "Texturing.h"
#include "Camera.h"
#include "Gizmos.h"
#include "Vertex.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "FlyCamera.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Texturing::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	LoadTexture("./textures/crate.png");
	GenerateQuad(5.0f);

	m_camera = new FlyCamera;
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() * 0.25, 1280.f / 720.f, 0.1f, 1000.f);

	return true;
}

void Texturing::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool Texturing::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}
	m_camera->Update(dt);

	return true;
}

void Texturing::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_ID);

	int projectionView_uniform = glGetUniformLocation(m_program_ID, "projection_view");

	glUniformMatrix4fv(projectionView_uniform, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	glActiveTexture(GL_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	int diffuse_location = glGetUniformLocation(m_program_ID, "diffuse");

	glUniform1i(diffuse_location, 0);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();

	Application::Draw();
}

void Texturing::LoadTexture(const char* filename)
{
	int width;
	int height;

	int channels;

	unsigned char* data = stbi_load(filename, &width, &height, &channels, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void Texturing::GenerateQuad(float size)
{
	VertexTexCoord vertex_data[4];
	vertex_data[0].position = vec4(-size, 0, -size, 1);
	vertex_data[1].position = vec4(-size, 0, size, 1);
	vertex_data[2].position = vec4(size, 0, size, 1);
	vertex_data[3].position = vec4(size, 0, -size, 1);

	vertex_data[0].tex_coord = vec2(0, 0);
	vertex_data[1].tex_coord = vec2(0, 1);
	vertex_data[2].tex_coord = vec2(1, 1);
	vertex_data[3].tex_coord = vec2(1, 0);

	unsigned int index_data[6] = { 0, 2, 1, 0, 3, 2 };

	glGenVertexArrays(1, &m_VAO);

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord) * 4, vertex_data, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Tex coord.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0); // Position.
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), (void*)sizeof(vec4)); // Tex coord.

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}