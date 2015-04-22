#include "Lighting.h"
#include "Gizmos.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "FlyCamera.h"
#include "Uitility.h"
#include "tiny_obj_loader.h"

bool Lighting::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	LoadShader("./Shaders/Lighting_Vertex.glsl", nullptr, "./Shaders/Light_Frag.glsl", &program_ID);

	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string err = tinyobj::LoadObj(shapes, materials, "./Models/Stanford/dragon.obj");

	if (err.size() != 0)
	{
		return false;
	}

	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	CreateOpenGLBuffers(shapes);

	ambient_Light = vec3(0.1f);
	light_Dir = vec3(0, -1, 0);
	light_Color = vec3(218.f / 255.f, 165 / 255.f, 32 / 255.f);
	material_Color = vec3(1);

	specular_Power = 15;

	return true;
}

void Lighting::ReloadShader()
{
	LoadShader("./Shaders/Lighting_Vertex.glsl", nullptr, "./Shaders/Light_Frag.glsl", &program_ID);
}

void Lighting::ShutDown()
{
	CleanUpOpenGLBuffers();

	Gizmos::destroy();

	Application::ShutDown();
}

bool Lighting::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	dt = glfwGetTime();
	glfwSetTime(0.0);
	m_camera->Update(dt);

	return true;
}

void Lighting::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glUseProgram(program_ID);

	int proj_View_Uniform = glGetUniformLocation(program_ID, "projection_view");
	glUniformMatrix4fv(proj_View_Uniform, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	int ambient_Uniform = glGetUniformLocation(program_ID, "ambient_Light");
	int light_Dir_Uniform = glGetUniformLocation(program_ID, "light_Dir");
	int light_Color_Uniform = glGetUniformLocation(program_ID, "light_Color");
	int material_Color_Uniform = glGetUniformLocation(program_ID, "material_Color");
	
	int eye_Pos_Uniform = glGetUniformLocation(program_ID, "eye_Pos");
	int specular_Uniform = glGetUniformLocation(program_ID, "spec_Power");

	glUniform3fv(ambient_Uniform, 1, (float*)&ambient_Light);
	glUniform3fv(light_Dir_Uniform, 1, (float*)&light_Dir);
	glUniform3fv(light_Color_Uniform, 1, (float*)&light_Color);
	glUniform3fv(material_Color_Uniform, 1, (float*)&material_Color);

	vec3 camera_Pos = m_camera->m_worldTransform[3].xyz;
	glUniform3fv(eye_Pos_Uniform, 1, (float*)&camera_Pos);
	glUniform1f(specular_Uniform, specular_Power);

	for (unsigned int mesh_Index = 0; mesh_Index < m_glData.size(); mesh_Index++)
	{
		glBindVertexArray(m_glData[mesh_Index].m_VAO);
		glDrawElements(GL_TRIANGLES, m_glData[mesh_Index].m_index_Count, GL_UNSIGNED_INT, 0);
	}
	
	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Lighting::CreateOpenGLBuffers(vector<tinyobj::shape_t> &shapes)
{
	m_glData.resize(shapes.size());

	for (unsigned int shape_index = 0; shape_index < shapes.size(); shape_index++)
	{
		vector<float> vertex_Data;

		unsigned int float_Counter = shapes[shape_index].mesh.positions.size();
		float_Counter += shapes[shape_index].mesh.normals.size();

		vertex_Data.reserve(float_Counter);
		vertex_Data.insert(vertex_Data.end(), shapes[shape_index].mesh.positions.begin(), shapes[shape_index].mesh.positions.end());
		vertex_Data.insert(vertex_Data.end(), shapes[shape_index].mesh.normals.begin(), shapes[shape_index].mesh.normals.end());

		m_glData[shape_index].m_index_Count = shapes[shape_index].mesh.indices.size();

		glGenVertexArrays(1, &m_glData[shape_index].m_VAO);
		glGenBuffers(1, &m_glData[shape_index].m_VBO);
		glGenBuffers(1, &m_glData[shape_index].m_IBO);

		glBindVertexArray(m_glData[shape_index].m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, m_glData[shape_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_Data.size() * sizeof(float), vertex_Data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glData[shape_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[shape_index].mesh.indices.size() * sizeof(unsigned int), shapes[shape_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * shapes[shape_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void Lighting::CleanUpOpenGLBuffers()
{
	glDeleteProgram(program_ID);

	for (unsigned int i = 0; i < m_glData.size(); i++)
	{
		glDeleteVertexArrays(1, &m_glData[i].m_VAO);
		glDeleteBuffers(1, &m_glData[i].m_VBO);
		glDeleteBuffers(1, &m_glData[i].m_IBO);
	}
}