#include "DeferredRendering.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"
#include "tiny_obj_loader.h"

using namespace std;

bool DeferredRendering::StartUp()
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

	m_timer = 0;

	BuildMesh();
	BuildGBuffer();
	BuildLightBuffer();
	BuildQuad();
	BuildCube();

	LoadShader("./Shaders/GBuffer_Vertex.glsl", 0, "./Shaders/GBuffer_Fragment.glsl", &m_gBuffer_Program);
	LoadShader("./Shaders/Composite_Vertex.glsl", 0, "./Shaders/Composite_Fragment.glsl", &m_composite_Program);
	LoadShader("./Shaders/Composite_Vertex.glsl", 0, "./Shaders/Directional_Light_Fragment.glsl", &m_dir_Light_Program);
	LoadShader("./Shaders/Point_Light_Vertex.glsl", 0, "./Shaders/Point_Light_Fragment.glsl", &m_point_Light_Program);

	glEnable(GL_CULL_FACE);

	return true;
}

void DeferredRendering::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool DeferredRendering::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	dt = glfwGetTime();
	m_timer += dt;
	glfwSetTime(0.0);
	m_camera->Update(dt);

	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	return true;
}

void DeferredRendering::Draw()
{
	// G-Buffer.
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer_FBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	vec4 clear_Color = vec4(0, 0, 0, 0);
	vec4 clear_Normal = vec4(0.5f, 0.5f, 0.5f, 0.5f);

	glClearBufferfv(GL_COLOR, 0, (float*)&clear_Color);
	glClearBufferfv(GL_COLOR, 1, (float*)&clear_Color);
	glClearBufferfv(GL_COLOR, 2, (float*)&clear_Normal);

	glUseProgram(m_gBuffer_Program);

	int view_Uni = glGetUniformLocation(m_gBuffer_Program, "view");
	int proj_View_Uni = glGetUniformLocation(m_gBuffer_Program, "proj_View");

	glUniformMatrix4fv(view_Uni, 1, GL_FALSE, (float*)&m_camera->m_viewTransform);
	glUniformMatrix4fv(proj_View_Uni, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	glBindVertexArray(m_bunny.m_VAO);
	glDrawElements(GL_TRIANGLES, m_bunny.m_index_Count, GL_UNSIGNED_INT, 0);
	// -----------------------------------

	// Setting up the light framebuffer.
	// Light Accumulation.
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_FBO);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	// Directional light.
	glUseProgram(m_dir_Light_Program);

	int pos_Tex_Uni = glGetUniformLocation(m_dir_Light_Program, "position_Texture");
	int norm_Tex_Uni = glGetUniformLocation(m_dir_Light_Program, "normal_Texture");

	glUniform1i(pos_Tex_Uni, 0);
	glUniform1i(norm_Tex_Uni, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_Texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_Texture);

	// Call RenderDirectionLight().
	RenderDirectionalLight(vec3(1, 0, 0), vec3(1, 1, 1));
	//RenderDirectionalLight(vec3(0, 1, 0), vec3(0, 1, 0));
	//RenderDirectionalLight(vec3(0, 0, 1), vec3(0, 0, 1));
	// -----------------------------------

	// Point light.
	glUseProgram(m_point_Light_Program);

	proj_View_Uni = glGetUniformLocation(m_point_Light_Program, "proj_View");
	pos_Tex_Uni = glGetUniformLocation(m_point_Light_Program, "position_Texture");
	norm_Tex_Uni = glGetUniformLocation(m_point_Light_Program, "normal_Texture");

	glUniformMatrix4fv(proj_View_Uni, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	glUniform1i(pos_Tex_Uni, 0);
	glUniform1i(norm_Tex_Uni, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_position_Texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normals_Texture);


	// Draw the point lights.
	RenderPointLight(vec3(sinf(m_timer) * 5, 3, cosf(m_timer) * 5), 5, vec3(1, 0, 0));
	RenderPointLight(vec3(sinf(m_timer) * -5, 3, cosf(m_timer) * -5), 5, vec3(0, 1, 0));
	RenderPointLight(vec3(3, sinf(m_timer) * 5, 3), 5, vec3(0, 0, 1));
	RenderPointLight(vec3(0, 1, 0), 5, vec3(1, 1, 0));

	glDisable(GL_BLEND);
	// -----------------------------------

	// Composite Pass.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_composite_Program);

	int albedo_Tex_Uni = glGetUniformLocation(m_composite_Program, "albedo_Texture");
	//pos_Tex_Uni = glGetUniformLocation(m_composite_Program, "position_Texture");
	//norm_Tex_Uni = glGetUniformLocation(m_composite_Program, "normals_Texture");
	int light_Tex_Uni = glGetUniformLocation(m_composite_Program, "light_Texture");

	glUniform1i(albedo_Tex_Uni, 0);
	//glUniform1i(pos_Tex_Uni, 1);
	//glUniform1i(norm_Tex_Uni, 2);
	glUniform1i(light_Tex_Uni, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedo_Texture);

	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, m_position_Texture);
	//
	//glActiveTexture(GL_TEXTURE2);
 	//glBindTexture(GL_TEXTURE_2D, m_normals_Texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_light_Texture);

	glBindVertexArray(m_screenspace_Quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	Gizmos::draw(m_camera->m_projectionView);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void DeferredRendering::BuildMesh()
{
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	tinyobj::LoadObj(shapes, materials, "./Models/stanford/bunny.obj");

	m_bunny.m_index_Count = shapes[0].mesh.indices.size();

	tinyobj::mesh_t* mesh = &shapes[0].mesh;

	vector<float> vertex_Data;
	vertex_Data.reserve(mesh->positions.size() + mesh->normals.size());

	vertex_Data.insert(vertex_Data.end(), mesh->positions.begin(), mesh->positions.end());
	vertex_Data.insert(vertex_Data.end(), mesh->normals.begin(), mesh->normals.end());

	glGenVertexArrays(1, &m_bunny.m_VAO);

	glGenBuffers(1, &m_bunny.m_VBO);
	glGenBuffers(1, &m_bunny.m_IBO);

	glBindVertexArray(m_bunny.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_bunny.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bunny.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, vertex_Data.size() * sizeof(float), vertex_Data.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* mesh->indices.size(), mesh->indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Normal.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)* mesh->positions.size()));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DeferredRendering::BuildGBuffer()
{
	// Create framebuffer.
	glGenFramebuffers(1, &m_gBuffer_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer_FBO);
	
	// Gen all tex,
	// Albedo, position, normal, depth(is renderbuffer).
	glGenTextures(1, &m_albedo_Texture);
	glBindTexture(GL_TEXTURE_2D, m_albedo_Texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_position_Texture);
	glBindTexture(GL_TEXTURE_2D, m_position_Texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenTextures(1, &m_normals_Texture);
	glBindTexture(GL_TEXTURE_2D, m_normals_Texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &m_gBuffer_Depth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gBuffer_Depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);

	// Attach textures to framebuffer.
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedo_Texture,	0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_position_Texture,	0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normals_Texture,	0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gBuffer_Depth);

	GLenum targets[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, targets);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR in creating gBuffer!\n");
	}
	
	// Check that it's working.
}

void DeferredRendering::BuildLightBuffer()
{
	// Create FBO.
	glGenFramebuffers(1, &m_light_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_light_FBO);

	// Create textures,
	// Light texture.
	glGenTextures(1, &m_light_Texture);
	glBindTexture(GL_TEXTURE_2D, m_light_Texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	// Attach thexture.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_light_Texture, 0);

	GLenum light_Targets = GL_COLOR_ATTACHMENT0;
	glDrawBuffers(1, &light_Targets);

	// Check for success.
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("ERROR in light framebuffer!\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRendering::BuildQuad()
{
	vec2 half_Texel = 1.0f / vec2(1280, 720);

	float vertex_Data[] =
	{
		-1, -1, 0, 1, half_Texel.x, half_Texel.y,
		-1, 1, 0, 1, half_Texel.x, 1.0f - half_Texel.y,
		1, 1, 0, 1, 1.0f - half_Texel.x, 1.0f - half_Texel.y,
		1, -1, 0, 1, 1.0f - half_Texel.x, half_Texel.y
	};

	unsigned int index_Data[] =
	{
		2, 1, 0, 3, 2, 0
	};

	glGenVertexArrays(1, &m_screenspace_Quad.m_VAO);
	glBindVertexArray(m_screenspace_Quad.m_VAO);

	glGenBuffers(1, &m_screenspace_Quad.m_VBO);
	glGenBuffers(1, &m_screenspace_Quad.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_screenspace_Quad.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_screenspace_Quad.m_IBO);

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

void DeferredRendering::BuildCube()
{
	float vertex_Data[] =
	{
		// Bottom 4 points.
		-1, -1, 1, 1,
		1, -1, 1, 1,
		1, -1, -1, 1,
		-1, -1, -1, 1,

		// Top 4 points.
		-1, 1, 1, 1,
		1, 1, 1, 1,
		1, 1, -1, 1,
		-1, 1, -1, 1,
	};

	unsigned int index_Data[] =
	{
		// Points of the cube.
		4, 5, 0,
		5, 1, 0,
		5, 6, 1,
		6, 2, 1,
		6, 7, 2,
		7, 3, 2,
		7, 4, 3,
		4, 0, 3,
		7, 6, 4,
		6, 5, 4,
		0, 1, 3,
		1, 2, 3,
	};

	glGenVertexArrays(1, &m_light_Cube.m_VAO);
	glBindVertexArray(m_light_Cube.m_VAO);

	glGenBuffers(1, &m_light_Cube.m_VBO);
	glGenBuffers(1, &m_light_Cube.m_IBO);

	glBindBuffer(GL_ARRAY_BUFFER, m_light_Cube.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_light_Cube.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_Data), vertex_Data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_Data), index_Data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float)* 4, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DeferredRendering::RenderDirectionalLight(vec3 _light_Dir, vec3 _light_Color)
{
	vec4 viewspace_Light_Dir = m_camera->m_viewTransform * vec4(_light_Dir, 0);

	int light_Dir_Uni = glGetUniformLocation(m_dir_Light_Program, "light_Dir");
	int light_Color_Uni = glGetUniformLocation(m_dir_Light_Program, "light_Color");

	glUniform3fv(light_Dir_Uni, 1, (float*)&viewspace_Light_Dir);
	glUniform3fv(light_Color_Uni, 1, (float*)&_light_Color);

	glBindVertexArray(m_screenspace_Quad.m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void DeferredRendering::RenderPointLight(vec3 _position, float _radius, vec3 _diffuse)
{
	vec4 view_Space_Pos = m_camera->m_viewTransform * vec4(_position, 1);

	int pos_Uniform = glGetUniformLocation(m_point_Light_Program, "light_Position");
	int view_Pos_Uniform = glGetUniformLocation(m_point_Light_Program, "light_View_Pos");
	int light_Diffuse_Uniform = glGetUniformLocation(m_point_Light_Program, "light_Diffuse");
	int light_Radius_Uniform = glGetUniformLocation(m_point_Light_Program, "light_Radius");

	glUniform3fv(pos_Uniform, 1, (float*)&_position);
	glUniform3fv(view_Pos_Uniform, 1, (float*)&view_Space_Pos);
	glUniform3fv(light_Diffuse_Uniform, 1, (float*)&_diffuse);
	glUniform1f(light_Radius_Uniform, _radius);

	glBindVertexArray(m_light_Cube.m_VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void DeferredRendering::ReloadShader()
{
	//LoadShader("./Shaders/GBuffer_Vertex.glsl", 0, "./Shaders/GBuffer_Fragment.glsl", &m_gBuffer_Program);
	//LoadShader("./Shaders/Composite_Vertex.glsl", 0, "./Shaders/Composite_Fragment.glsl", &m_composite_Program);
	//LoadShader("./Shaders/Composite_Vertex.glsl", 0, "./Shaders/Directional_Light_Fragment.glsl", &m_dir_Light_Program);
	LoadShader("./Shaders/Point_Light_Vertex.glsl", 0, "./Shaders/Point_Light_Fragment.glsl", &m_point_Light_Program);
}