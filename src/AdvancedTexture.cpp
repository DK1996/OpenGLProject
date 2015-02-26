#include "AdvancedTexture.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void OnMouseButton(GLFWwindow* _window, int _button, int _pressed, int _mod_Keys)
{
	TwEventMouseButtonGLFW(_button, _pressed);
}

void OnMousePosition(GLFWwindow* _window, double _x, double _y)
{
	TwEventMousePosGLFW((int)_x, (int)_y);
}

void OnMouseScroll(GLFWwindow* _window, double _x, double _y)
{
	TwEventMouseWheelGLFW((int)_y);
}

void OnKey(GLFWwindow* _window, int _key, int _scancode, int _pressed, int _mod_Keys)
{
	TwEventKeyGLFW(_key, _pressed);
}

void OnChar(GLFWwindow* _window, unsigned int _c)
{
	TwEventCharGLFW(_c, GLFW_PRESS);
}

void OnWindowResize(GLFWwindow* _window, int _width, int _height)
{
	TwWindowSize(_width, _height);
	glViewport(0, 0, _width, _height);
}

bool AdvancedTexture::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);

	m_background_Color = vec4(0.3f, 0.3f, 0.3f, 1);

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

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	m_bar = TwNewBar("My Epic New Bar Broseph!");

	TwAddVarRW(m_bar, "Clear Colour", TW_TYPE_COLOR4F, &m_background_Color, "");
	TwAddVarRW(m_bar, "Light Direction", TW_TYPE_DIR3F, &m_light_Dir, "group=Light");
	TwAddVarRW(m_bar, "Light Colour", TW_TYPE_COLOR3F, &m_light_Color, "group=Light");
	TwAddVarRW(m_bar, "Spec Power", TW_TYPE_FLOAT, &m_spec_Power, "group=Light");
	TwAddVarRW(m_bar, "Draw Gizmos", TW_TYPE_BOOL8, &m_draw_Gizmos, "");
	TwAddVarRO(m_bar, "FPS", TW_TYPE_FLOAT, &m_fps, "");

	return true;
}

void AdvancedTexture::ShutDown()
{
	Gizmos::destroy();

	TwDeleteAllBars();
	TwTerminate();

	Application::ShutDown();
}

bool AdvancedTexture::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	glClearColor(m_background_Color.x, m_background_Color.y, m_background_Color.z, m_background_Color.w);

	m_light_Dir = (rotate(dt, vec3(0, 1, 0)) * vec4(m_light_Dir, 0)).xyz;

	m_fps = 1 / dt;

	m_camera->Update(dt);

	return true;
}

void AdvancedTexture::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_program_ID);

	int proj_View_Uniform	= glGetUniformLocation(m_program_ID, "projection_view");
	int ambient_Uniform		= glGetUniformLocation(m_program_ID, "ambient_Light");
	int light_Dir_Uniform	= glGetUniformLocation(m_program_ID, "light_Dir");
	int light_Color_Uniform	= glGetUniformLocation(m_program_ID, "light_Color");
	int eye_Pos_Uniform		= glGetUniformLocation(m_program_ID, "eye_Pos");
	int specular_Uniform	= glGetUniformLocation(m_program_ID, "spec_Power");
	int diff_Location		= glGetUniformLocation(m_program_ID, "diff_Tex");
	int norm_Location		= glGetUniformLocation(m_program_ID, "norm_Tex");
	int spec_Location		= glGetUniformLocation(m_program_ID, "spec_Tex");

	vec3 camera_Pos = m_camera->m_worldTransform[3].xyz;

	glUniformMatrix4fv(proj_View_Uniform,	1, GL_FALSE, (float*)&m_camera->m_projectionView);
	glUniform3fv(ambient_Uniform,			1, (float*)&m_ambient_Light);
	glUniform3fv(light_Dir_Uniform,			1, (float*)&m_light_Dir);
	glUniform3fv(light_Color_Uniform,		1, (float*)&m_light_Color);
	glUniform3fv(eye_Pos_Uniform,			1, (float*)&camera_Pos);
	glUniform1f(specular_Uniform, m_spec_Power);

	glUniform1i(diff_Location, 0);
	glUniform1i(norm_Location, 1);
	glUniform1i(spec_Location, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_diffuse_Texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normal_Texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_spec_Texture);

	glBindVertexArray(m_quad.m_VAO);
	glDrawElements(GL_TRIANGLES, m_quad.m_index_Count, GL_UNSIGNED_INT, 0);

	if (m_draw_Gizmos == true)
	{
		Gizmos::draw(m_camera->m_projectionView);
	}

	TwDraw();

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

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,	sizeof(VectexNormal), 0);							// Position.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,	sizeof(VectexNormal), (void*)(sizeof(vec4) * 1));	// Normal.
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE,	sizeof(VectexNormal), (void*)(sizeof(vec4) * 2));	// Tangent.
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE,	sizeof(VectexNormal), (void*)(sizeof(vec4) * 3));	// Tex Coord.

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