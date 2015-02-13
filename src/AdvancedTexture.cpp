#include "AdvancedTexture.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool AdvancedTexture::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	GenQuad(5.0f);
	LoadTexture();
	LoadShader("./Shaders/Normal_Mapped_Vertex.glsl", "./Shaders/Normal_Mapped_Fragment.glsl", &m_program_ID);

	m_ambient_Light = vec3(0.1f);
	m_light_Color = vec3(0.7f);
	m_light_Dir = normalize(vec3(-1, -10, 0));

	m_spec_Power = 15;

	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	return true;
}

void AdvancedTexture::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool AdvancedTexture::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	m_light_Dir = (rotate(dt, vec3(0, 1, 0)) * vec4(m_light_Dir, 0)).xyz;

	m_camera->Update(dt);

	return true;
}

void AdvancedTexture::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_ID);

	int proj_View_Uniform = glGetUniformLocation(m_program_ID, "projection_view");
	glUniformMatrix4fv(proj_View_Uniform, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	int ambient_Uniform		= glGetUniformLocation(m_program_ID, "ambient_Light");
	int light_Dir_Uniform	= glGetUniformLocation(m_program_ID, "light_Dir");
	int light_Color_Uniform	= glGetUniformLocation(m_program_ID, "light_Color");
	int eye_Pos_Uniform		= glGetUniformLocation(m_program_ID, "eye_Pos");
	int specular_Uniform	= glGetUniformLocation(m_program_ID, "spec_Power");

	glUniform3fv(ambient_Uniform,		1, (float*)&m_ambient_Light);
	glUniform3fv(light_Dir_Uniform,		1, (float*)&m_light_Dir);
	glUniform3fv(light_Color_Uniform,	1, (float*)&m_light_Color);

	vec3 camera_Pos = m_camera->m_worldTransform[3].xyz;
	glUniform3fv(eye_Pos_Uniform, 1, (float*)&camera_Pos);
	glUniform1f(specular_Uniform, m_spec_Power);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_Texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normal_Texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_spec_Texture);

	int diff_Location = glGetUniformLocation(m_program_ID, "diff_Tex");
	int norm_Location = glGetUniformLocation(m_program_ID, "norm_Tex");
	int spec_Location = glGetUniformLocation(m_program_ID, "spec_Tex");

	glUniform1i(diff_Location, 0);
	glUniform1i(norm_Location, 1);
	glUniform1i(spec_Location, 2);

	glBindVertexArray(m_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, m_quad.m_index_Count, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void AdvancedTexture::GenQuad(float size)
{
	VectexNormal vertex_data[4];
	vertex_data[0].position = vec4(-size, 0, -size, 1);
	vertex_data[1].position = vec4(-size, 0, size, 1);
	vertex_data[2].position = vec4(size, 0, size, 1);
	vertex_data[3].position = vec4(size, 0, -size, 1);

	vertex_data[0].normal = vec4(0, 1, 0, 0);
	vertex_data[1].normal = vec4(0, 1, 0, 0);
	vertex_data[2].normal = vec4(0, 1, 0, 0);
	vertex_data[3].normal = vec4(0, 1, 0, 0);

	vertex_data[0].tangent = vec4(1, 0, 0, 0);
	vertex_data[0].tangent = vec4(1, 0, 0, 0);
	vertex_data[0].tangent = vec4(1, 0, 0, 0);
	vertex_data[0].tangent = vec4(1, 0, 0, 0);

	vertex_data[0].tex_Coord = vec2(0, 0);
	vertex_data[1].tex_Coord = vec2(0, 1);
	vertex_data[2].tex_Coord = vec2(1, 1);
	vertex_data[3].tex_Coord = vec2(1, 0);

	unsigned int index_data[6] = { 0, 2, 1, 0, 3, 2 };

	glGenVertexArrays(1, &m_quad.m_VAO);

	glGenBuffers(1, &m_quad.m_VBO);
	glGenBuffers(1, &m_quad.m_IBO);

	glBindVertexArray(m_quad.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_quad.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord)* 4, vertex_data, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quad.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6, index_data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Normal.
	glEnableVertexAttribArray(2); // Tangent.
	glEnableVertexAttribArray(3); // Tex Coord.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VectexNormal), 0); // Position.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(VectexNormal), (void*)(sizeof(vec4) * 1)); // Normal.
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(VectexNormal), (void*)(sizeof(vec4) * 2)); // Tangent.
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(VectexNormal), (void*)(sizeof(vec4) * 3)); // Tex Coord.

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void AdvancedTexture::LoadTexture()
{
	int width, height, channels;

	unsigned char* data;

	data = stbi_load("./textures/rock_diffuse.tga", &width, &height, &channels, STBI_default);

	glGenTextures(1, &m_diffuse_Texture);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_Texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./textures/rock_normal.tga", &width, &height, &channels, STBI_default);

	glGenTextures(1, &m_normal_Texture);
	glBindTexture(GL_TEXTURE_2D, m_normal_Texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./textures/rock_specular.tga", &width, &height, &channels, STBI_default);

	glGenTextures(1, &m_spec_Texture);
	glBindTexture(GL_TEXTURE_2D, m_spec_Texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}