#include "Shadows.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"
#include "tiny_obj_loader.h"

bool Shadows::StartUp()
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
	
	LoadShader("./Shaders/Diffuse_Shadowed_Vertex.glsl", 0, "./Shaders/Diffuse_Shadowed_Fragment/glsl", &m_diffuse_Shadowed_Program);

	LoadShader("./Shaders/Shadow_Map_Vertex.glsl", 0, "./Shaders/Shadow_Map_Fragment/glsl", &m_shadow_Map_Program);

	BuildMeshes();
	BuildShadowMaps();

	return true;
}

void Shadows::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool Shadows::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	dt = glfwGetTime();
	glfwSetTime(0.0);
	m_camera->Update(dt);

	return true;
}

void Shadows::Draw()
{
	// Rendering the Shadow.
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glViewport(0, 0, 1024, 1024);

	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shadow_Map_Program);

	int light_Matrix_Location = glGetUniformLocation(m_shadow_Map_Program, "light_Matrix");

	vec3 light_Dir = normalize(vec3(-1, -2.5f, -1));

	mat4 light_Proj = ortho<float>(-10, 10, -10, 10, -10, 100);
	mat4 light_View = lookAt(-light_Dir, vec3(0), vec3(0, 1, 0));

	mat4 light_Matrix = light_Proj * light_View;

	glUniformMatrix4fv(light_Matrix_Location, 1, GL_FALSE, (float*)&light_Matrix);

	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, m_bunny.m_index_Count, GL_UNSIGNED_INT, 0);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_index_Count, GL_UNSIGNED_INT, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);

	// -----------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_diffuse_Shadowed_Program);

	mat4 offset_Scale = mat4(0.5f, 0.0f, 0.0f, 0.0f,
							 0.0f, 0.5f, 0.0f, 0.0f,
							 0.0f, 0.0f, 0.5f, 0.0f,
							 0.5f, 0.5f, 0.5f, 0.1f);

	int view_Proj_Uniform = glGetUniformLocation(m_diffuse_Shadowed_Program, "view_Projection");
	glUniformMatrix4fv(view_Proj_Uniform, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	int light_Dir_Uniform = glGetUniformLocation(m_diffuse_Shadowed_Program, "light_dir");
	
	glUniform3fv(light_Dir_Uniform, 1, (float*)&light_Dir);

	int light_Mat_Uni = glGetUniformLocation(m_diffuse_Shadowed_Program, "light_Matrix");
	glUniformMatrix4fv(light_Mat_Uni, 1, GL_FALSE, (float*)&light_Matrix);

	int shadow_Map_Location = glGetUniformLocation(m_diffuse_Shadowed_Program, "shadow_Map");
	glUniform1i(shadow_Map_Location, 0);

	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, m_bunny.m_index_Count, GL_UNSIGNED_INT, 0);

	glBindVertexArray(m_plane.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane.m_index_Count, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void Shadows::BuildMeshes()
{
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err = tinyobj::LoadObj(shapes, materials, "./Models/stanford/bunny.obj");

	if (err.size() != 0)
	{
		return;
	}

	m_bunny.m_index_Count = shapes[0].mesh.indices.size();

	tinyobj::mesh_t* mesh = &shapes[0].mesh;

	std::vector<float> vertex_Data;
	vertex_Data.reserve(mesh->positions.size() + mesh->normals.size());

	vertex_Data.insert(vertex_Data.begin(), mesh->positions.begin(), mesh->positions.end());
	vertex_Data.insert(vertex_Data.begin(), mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &m_bunny.m_VAO);

	glGenBuffers(1, &m_bunny.m_VBO);
	glGenBuffers(1, &m_bunny.m_IBO);

	glBindVertexArray(m_bunny.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bunny.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunny.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, vertex_Data.size() * sizeof(float), vertex_Data.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(unsigned int) * mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Normal.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_plane.m_VAO);
	glBindVertexArray(m_plane.m_VAO);

	glGenBuffers(1, &m_plane.m_VBO);
	glGenBuffers(1, &m_plane.m_IBO);

	float plane_Vertex_Data[] =
	{
		-10, 0, -10, 1,		0, 1, 0, 0,
		 10, 0, -10, 1,		0, 1, 0, 0,
		 10, 0,  10, 1,		0, 1, 0, 0,
		-10, 0,  10, 1,		0, 1, 0, 0,
	};

	unsigned int plane_index_Data[] =
	{
		0, 1, 2, 0, 2, 3
	};

	m_plane.m_index_Count = 6;

	glBindBuffer(GL_ARRAY_BUFFER, m_plane.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(plane_Vertex_Data), vertex_Data.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(plane_index_Data)* mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Normal.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Shadows::BuildShadowMaps()
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_FBO_Depth);
	glBindTexture(GL_TEXTURE_2D, m_FBO_Depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_FBO_Depth, 0);

	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}