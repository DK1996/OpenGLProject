#include "Assignment.h"
#include "stb_image.h"


// AntTweakBar controls.
void OnMouseButton(GLFWwindow*, int b, int a, int m)
{
	TwEventMouseButtonGLFW(b, a);
}

void OnMousePosition(GLFWwindow*, double x, double y)
{
	TwEventMousePosGLFW((int)x, (int)y);
}

void OnMouseScroll(GLFWwindow*, double x, double y)
{
	TwEventMouseWheelGLFW((int)y);
}

void OnKey(GLFWwindow*, int k, int s, int a, int m)
{
	TwEventKeyGLFW(k, a);
}

void OnChar(GLFWwindow*, unsigned int c) 
{
	TwEventCharGLFW(c, GLFW_PRESS);
}

void OnWindowResize(GLFWwindow*, int w, int h) 
{
	TwWindowSize(w, h);
	glViewport(0, 0, w, h);
}
// ----------------------------------------------------------------------------

bool Assignment::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	// Setting global values here...
	// Setting the values for the procedural generation.
	m_real_Dims		= 25.0f;
	m_dims			= 64;
	m_octaves		= 5;
	m_persistance	= 0.3f;

	m_scale			= (1.0f / ivec2(m_dims, m_dims).x * 3.0f); /*Sets the jitter.*/
	// -----------------------------------

	// Setting the values for the animation meshes.
	m_fps = 20.f;
	// -----------------------------------

	// Assigning a model to the fbx file.
	m_fbx_File = new FBXFile();
	m_fbx_File->load("./Models/characters/Enemyelite/EnemyElite.fbx");
	m_fbx_File->initialiseOpenGLTextures();

	m_fbx_File_2 = new FBXFile();
	m_fbx_File_2->load("./Models/characters/Pyro/pyro.fbx");
	m_fbx_File_2->initialiseOpenGLTextures();
	// -----------------------------------

	// Assigning the light values.
	ambient_Light = vec3(0.1f);
	light_Dir = vec3(0.1, -0.53, -0.83);
	light_Color = vec3(1);

	specular_Power = 15;
	// -----------------------------------

	//m_mesh_Position = vec4(0);

	m_timer = 0;

	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	// Makes the all the buttons work for the AntTweakBar work.
	glfwSetMouseButtonCallback(m_window, OnMouseButton);
	glfwSetCursorPosCallback(m_window, OnMousePosition);
	glfwSetScrollCallback(m_window, OnMouseScroll);
	glfwSetKeyCallback(m_window, OnKey);
	glfwSetCharCallback(m_window, OnChar);
	glfwSetWindowSizeCallback(m_window, OnWindowResize);
	// -----------------------------------

	// Setting yp the FlyCamera.
	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	// Calling the grid and the height map for the gid.
	BuildGrid(vec2(m_real_Dims, m_real_Dims), ivec2(m_dims, m_dims));
	BuildPerlinTexture(ivec2(m_dims, m_dims), m_octaves, m_persistance);
	// -----------------------------------

	// Calling the animated mesh.
	GenerateGLMesh(m_fbx_File);
	GenerateGLMesh2(m_fbx_File_2);
	// -----------------------------------

	// Loading the shaders here...
	LoadShader("./Shaders/Perlin_Vertex.glsl", 0, "./Shaders/Perlin_Fragment.glsl", &m_perlin_Program_ID);	// Shaders for the procedural generation.
	LoadShader("./Shaders/Skinned_Vertex.glsl", 0, "./Shaders/Skinned_Fragment.glsl", &m_fbx_Program_ID);	// Shaders for the animation meshes.
	LoadShader("./Shaders/Skinned_Vertex.glsl", 0, "./Shaders/Skinned_Fragment.glsl", &m_fbx_Program_ID_2);
	// -----------------------------------

	LoadTexture();

	// Calling the GUI function.
	AntTweakButtons();
	// -----------------------------------

	return true;
}

void Assignment::ShutDown()
{
	m_fbx_File->unload();
	delete m_fbx_File;

	TwDeleteAllBars();
	TwTerminate();

	Gizmos::destroy();

	Application::ShutDown();
}

bool Assignment::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	m_dt = glfwGetTime();
	glfwSetTime(0.0);
	m_camera->Update(m_dt);

	m_timer += (float)m_dt;

	// Updating the skeleton for the first animated mesh.
	FBXSkeleton*	skeleton	= m_fbx_File->getSkeletonByIndex(0);
	FBXAnimation*	animation	= m_fbx_File->getAnimationByIndex(0);
	
	EvaluateSkeleton(animation, skeleton, m_timer);
	
	/*for (unsigned int i = 0; i < skeleton->m_boneCount; i++)
	{
		skeleton->m_nodes[i]->updateGlobalTransform();
	
		mat4 node_Global	= skeleton->m_nodes[i]->m_globalTransform;
		vec3 node_Pos		= node_Global[3].xyz;
	
		Gizmos::addAABBFilled(node_Pos, vec3(10.f), vec4(1, 0, 0, 1), &node_Global);
	
		if (skeleton->m_nodes[i]->m_parent != nullptr)
		{
			vec3 parent_Pos = skeleton->m_nodes[i]->m_parent->m_globalTransform[3].xyz;
	
			Gizmos::addLine(node_Pos, parent_Pos, vec4(0, 1, 0, 1));
		}
	}*/
	// -----------------------------------

	// Updating the skeleton for the second animated mesh.
	FBXSkeleton*	skeleton_2 = m_fbx_File_2->getSkeletonByIndex(0);
	FBXAnimation*	animation_2 = m_fbx_File_2->getAnimationByIndex(0);

	EvaluateSkeleton(animation_2, skeleton_2, m_timer);

	/*for (unsigned int i = 0; i < skeleton_2->m_boneCount; i++)
	{
		skeleton_2->m_nodes[i]->updateGlobalTransform();

		mat4 node_Global = skeleton_2->m_nodes[i]->m_globalTransform;
		vec3 node_Pos = node_Global[3].xyz;

		Gizmos::addAABBFilled(node_Pos, vec3(10.f), vec4(1, 0, 0, 1), &node_Global);

		if (skeleton_2->m_nodes[i]->m_parent != nullptr)
		{
			vec3 parent_Pos = skeleton_2->m_nodes[i]->m_parent->m_globalTransform[3].xyz;

			Gizmos::addLine(node_Pos, parent_Pos, vec4(0, 1, 0, 1));
		}
	}*/
	// -----------------------------------

	// Reloading the terrian when the reload button is clicked.
	if (m_reload)
	{
		m_reload = !m_reload;
		BuildGrid(vec2(m_real_Dims, m_real_Dims), ivec2(m_dims, m_dims));
		BuildPerlinTexture(ivec2(m_dims, m_dims), m_octaves, m_persistance);
	}
	// -----------------------------------
	
	// Reloads the shaders when the reload button is pressed.
	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}
	// -----------------------------------

	return true;
}

void Assignment::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	// Drawing the Terrian.
	glUseProgram(m_perlin_Program_ID);

	int view_Proj_Uni	= glGetUniformLocation(m_perlin_Program_ID, "view_Proj");
	glUniformMatrix4fv(view_Proj_Uni, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	int tex_Uni			= glGetUniformLocation(m_perlin_Program_ID, "perlin_Texture");
	glUniform1i(tex_Uni, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_Texture);

	int terrian_Tex_Uni	= glGetUniformLocation(m_perlin_Program_ID, "diffuse");
	glUniform1i(terrian_Tex_Uni, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_terrian_Texture);

	// Drawing the lighting.
	int ambient_Uni = glGetUniformLocation(m_perlin_Program_ID, "ambient_Light");
	int light_Dir_Uni = glGetUniformLocation(m_perlin_Program_ID, "light_Dir");
	int light_Color_Uni = glGetUniformLocation(m_perlin_Program_ID, "light_Color");

	int eye_Pos_Uni = glGetUniformLocation(m_perlin_Program_ID, "eye_Pos");
	int specular_Uni = glGetUniformLocation(m_perlin_Program_ID, "spec_Power");

	glUniform3fv(ambient_Uni, 1, (float*)&ambient_Light);
	glUniform3fv(light_Dir_Uni, 1, (float*)&light_Dir);
	glUniform3fv(light_Color_Uni, 1, (float*)&light_Color);

	vec3 camera_Pos = m_camera->m_worldTransform[3].xyz;
	glUniform3fv(eye_Pos_Uni, 1, (float*)&camera_Pos);
	glUniform1f(specular_Uni, specular_Power);
	// -----------------------------------

	glBindVertexArray(m_plane_Mesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane_Mesh.m_index_Count, GL_UNSIGNED_INT, 0);
	// -----------------------------------

	// Drawing the Animated mesh.
	// Animated mesh #1.
	glUseProgram(m_fbx_Program_ID);

	int proj_View_Uni		= glGetUniformLocation(m_fbx_Program_ID, "projection_view");
	glUniformMatrix4fv(proj_View_Uni, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	int diffuse_Uni			= glGetUniformLocation(m_fbx_Program_ID, "diffuse");
	glUniform1i(diffuse_Uni, 0);
	
	// Drawing the lighting.
	ambient_Uni = glGetUniformLocation(m_fbx_Program_ID, "ambient_Light");
	light_Dir_Uni = glGetUniformLocation(m_fbx_Program_ID, "light_Dir");
	light_Color_Uni = glGetUniformLocation(m_fbx_Program_ID, "light_Color");

	eye_Pos_Uni = glGetUniformLocation(m_fbx_Program_ID, "eye_Pos");
	specular_Uni = glGetUniformLocation(m_fbx_Program_ID, "spec_Power");

	glUniform3fv(ambient_Uni, 1, (float*)&ambient_Light);
	glUniform3fv(light_Dir_Uni, 1, (float*)&light_Dir);
	glUniform3fv(light_Color_Uni, 1, (float*)&light_Color);

	camera_Pos = m_camera->m_worldTransform[3].xyz;
	glUniform3fv(eye_Pos_Uni, 1, (float*)&camera_Pos);
	glUniform1f(specular_Uni, specular_Power);
	// -----------------------------------
	       

	/*int pos_Uni				= glGetUniformLocation(m_fbx_Program_ID, "final_Position");
	glUniform4fv(pos_Uni, 1, (float*)&m_mesh_Position);*/

	FBXSkeleton* skeleton	= m_fbx_File->getSkeletonByIndex(0);
	skeleton->updateBones();

	UpdateBones(skeleton);

	int bones_Uni			= glGetUniformLocation(m_fbx_Program_ID, "bones");
	glUniformMatrix4fv(bones_Uni, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	for (unsigned int i = 0; i < m_meshes.size(); i++)
	{
		FBXMeshNode* current_Mesh	= m_fbx_File->getMeshByIndex(i);
		FBXMaterial* mesh_Material	= current_Mesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh_Material->textures[FBXMaterial::DiffuseTexture]->handle);

		mat4 world_Transform	= current_Mesh->m_globalTransform;
		int world_Uni			= glGetUniformLocation(m_fbx_Program_ID, "world");
		glUniformMatrix4fv(world_Uni, 1, GL_FALSE, (float*)&world_Transform);

		glBindVertexArray(m_meshes[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes[i].m_index_Count, GL_UNSIGNED_INT, 0);
	}
	// -----------------------------------

	// Animated mesh #2.
	glUseProgram(m_fbx_Program_ID_2);

	proj_View_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "projection_view");
	glUniformMatrix4fv(proj_View_Uni, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	diffuse_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "diffuse");
	glUniform1i(diffuse_Uni, 0);

	int offset_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "offset");
	glUniform3fv(offset_Uni, 1, (float*)&(vec3(1000)));

	// Drawing the light.

	//glUseProgram(light_Program_ID);
	//proj_View_Uni = glGetUniformLocation(light_Program_ID, "projection_view");
	//glUniformMatrix4fv(proj_View_Uni, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	ambient_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "ambient_Light");
	light_Dir_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "light_Dir");
	light_Color_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "light_Color");
	
	eye_Pos_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "eye_Pos");
	specular_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "spec_Power");

	glUniform3fv(ambient_Uni, 1, (float*)&ambient_Light);
	glUniform3fv(light_Dir_Uni, 1, (float*)&light_Dir);
	glUniform3fv(light_Color_Uni, 1, (float*)&light_Color);
	
	camera_Pos = m_camera->m_worldTransform[3].xyz;
	glUniform3fv(eye_Pos_Uni, 1, (float*)&camera_Pos);
	glUniform1f(specular_Uni, specular_Power);
	// -----------------------------------

	//int pos_Uni				= glGetUniformLocation(m_fbx_Program_ID, "final_Position");
	//glUniform4fv(pos_Uni, 1, (float*)&m_mesh_Position);

	skeleton = m_fbx_File_2->getSkeletonByIndex(0);
	skeleton->updateBones();

	UpdateBones(skeleton);

	bones_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "bones");
	glUniformMatrix4fv(bones_Uni, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	for (unsigned int i = 0; i < m_meshes_2.size(); i++)
	{
		FBXMeshNode* current_Mesh = m_fbx_File_2->getMeshByIndex(i);
		FBXMaterial* mesh_Material = current_Mesh->m_material;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh_Material->textures[FBXMaterial::DiffuseTexture]->handle);

		mat4 world_Transform = current_Mesh->m_globalTransform;
		int world_Uni = glGetUniformLocation(m_fbx_Program_ID_2, "world");
		glUniformMatrix4fv(world_Uni, 1, GL_FALSE, (float*)&world_Transform);

		glBindVertexArray(m_meshes_2[i].m_VAO);
		glDrawElements(GL_TRIANGLES, m_meshes_2[i].m_index_Count, GL_UNSIGNED_INT, 0);
	}
	// -----------------------------------
	// -----------------------------------

	TwDraw();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

// Procedural Generation functions.
void Assignment::BuildGrid(vec2 _real_Dims, ivec2 _dims)
{
	// Compute how many vertices we need.
	unsigned int vertex_Count = (_dims.x + 1) * (_dims.y + 1);

	// Allocate vertex data.
	VertexTexCoord* vertex_Data = new VertexTexCoord[vertex_Count];

	// Compute how mant indices we need.
	unsigned int index_Count = _dims.x * _dims.y * 6;

	// Allocate index data.
	unsigned int* index_Data = new unsigned int[index_Count];

	// Two nested for loops,
	// Generate vertex data.
	float curr_y = -_real_Dims.y / 2.0f;
	for (int y = 0; y < _dims.y + 1; y++)
	{
		float curr_x = -_real_Dims.x / 2.0f;
		for (int x = 0; x < _dims.x + 1; x++)
		{
			// Inside we create our points.
			vertex_Data[y * (_dims.x + 1) + x].position = vec4(curr_x, 0, curr_y, 1);
			vertex_Data[y * (_dims.x + 1) + x].tex_coord = vec2(x / (float)_dims.x, (float)y / (float)_dims.y);

			curr_x += _real_Dims.x / (float)_dims.x;
		}

		curr_y += _real_Dims.y / (float)_dims.y;
	}

	// Generate index data.
	int curr_Index = 0;
	for (int y = 0; y < _dims.y; y++)
	{
		for (int x = 0; x < _dims.x; x++)
		{
			// Create our 6 indices here!
			index_Data[curr_Index++] = y		* (_dims.x + 1) + x;
			index_Data[curr_Index++] = (y + 1)	* (_dims.x + 1) + x;
			index_Data[curr_Index++] = (y + 1)	* (_dims.x + 1) + (x + 1);

			index_Data[curr_Index++] = (y + 1)	* (_dims.x + 1) + (x + 1);
			index_Data[curr_Index++] = y		* (_dims.x + 1) + (x + 1);
			index_Data[curr_Index++] = y		* (_dims.x + 1) + x;
		}
	}

	m_plane_Mesh.m_index_Count = index_Count;

	// Create VerexArrayObject, buffers, etc.
	glGenVertexArrays(1, &m_plane_Mesh.m_VAO);
	glGenBuffers(1, &m_plane_Mesh.m_VBO);
	glGenBuffers(1, &m_plane_Mesh.m_IBO);

	// Bind and fill buffers.
	glBindVertexArray(m_plane_Mesh.m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_plane_Mesh.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_plane_Mesh.m_IBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexTexCoord)* vertex_Count, vertex_Data, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* index_Count, index_Data, GL_STATIC_DRAW);

	// Tell openGL about our vertex structure.
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexTexCoord), (void*)sizeof(vec4));

	// Unbind stuff.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Free vertex and index data.
	delete[] vertex_Data;
	delete[] index_Data;
}

void Assignment::BuildPerlinTexture(ivec2 _dims, int _octaves, float _persistance)
{
	// Allocate memory for perlin data.
	m_perlin_Data = new float[_dims.x * _dims.y];

	// Loop through all the pixels.
	for (int y = 0; y < _dims.y; y++)
	{
		for (int x = 0; x < _dims.x; x++)
		{
			m_amplitute = 1;
			m_freq = 1;

			// Generate our perlin noise.
			m_perlin_Data[y * _dims.x + x] = 0;

			// Loop over number of octaves. (However many get passed in)
			for (int o = 0; o < _octaves; o++)
			{
				// Each octave will have double the frequency of the pervious one.
				// To do this we scale our points.
				float perlin_Sample = glm::perlin(vec2(x, y) * m_scale * m_freq) * 0.5f + 0.5f;

				// Each subsequent octave will contribute less and less to the final height.
				// We do this by muliplying by a smaller and smaller number.
				perlin_Sample *= m_amplitute;

				// Call perlin function
				m_perlin_Data[y * _dims.x + x] += perlin_Sample;

				m_amplitute *= _persistance;
				m_freq *= 2.0f;
			}
		}
	}

	// Generate textures handles.
	glGenTextures(1, &m_perlin_Texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_Texture);

	// Pass perlin data to the texture.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _dims.x, _dims.y, 0, GL_RED, GL_FLOAT, m_perlin_Data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
}
// ----------------------------------------------------------------------------

// Animation Mesh functions.
void Assignment::GenerateGLMesh(FBXFile* _fbx)
{
	unsigned int mesh_Count = _fbx->getMeshCount();

	// Generating the meshes for the first animated object.
	m_meshes.resize(mesh_Count);

	for (unsigned int  mesh_Index = 0; mesh_Index < mesh_Count; mesh_Index++)
	{
		FBXMeshNode* current_Mesh = _fbx->getMeshByIndex(mesh_Index);

		m_meshes[mesh_Index].m_index_Count = current_Mesh->m_indices.size();

		glGenVertexArrays(	1, &m_meshes[mesh_Index].m_VAO );
		glGenBuffers(		1, &m_meshes[mesh_Index].m_VBO );
		glGenBuffers(		1, &m_meshes[mesh_Index].m_IBO );

		glBindVertexArray(						m_meshes[mesh_Index].m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER,			m_meshes[mesh_Index].m_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,	m_meshes[mesh_Index].m_IBO);

		glBufferData(GL_ARRAY_BUFFER,			sizeof(FBXVertex)*		current_Mesh->m_vertices.size(),	current_Mesh->m_vertices.data(),	GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,	sizeof(unsigned int)*	current_Mesh->m_indices.size(),		current_Mesh->m_indices.data(),		GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Position.
		glEnableVertexAttribArray(1); // Tex Coord.
		glEnableVertexAttribArray(2); // Bone Indices.
		glEnableVertexAttribArray(3); // Bone Weights.
		glEnableVertexAttribArray(4); // Normals.

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	// -----------------------------------
}

// The #2 animated mesh...
void Assignment::GenerateGLMesh2(FBXFile* _fbx)
{
	unsigned int mesh_Count = _fbx->getMeshCount();

	m_meshes_2.resize(mesh_Count);

	// Generating the meshes for the second animated object,
	for (unsigned int mesh_Index = 0; mesh_Index < mesh_Count; mesh_Index++)
	{
		FBXMeshNode* current_Mesh = _fbx->getMeshByIndex(mesh_Index);

		m_meshes_2[mesh_Index].m_index_Count = current_Mesh->m_indices.size();

		glGenVertexArrays(	1, &m_meshes_2[mesh_Index].m_VAO );
		glGenBuffers(		1, &m_meshes_2[mesh_Index].m_VBO );
		glGenBuffers(		1, &m_meshes_2[mesh_Index].m_IBO );

		glBindVertexArray(						m_meshes_2[mesh_Index].m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER,			m_meshes_2[mesh_Index].m_VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,	m_meshes_2[mesh_Index].m_IBO);

		glBufferData(GL_ARRAY_BUFFER,			sizeof(FBXVertex)*		current_Mesh->m_vertices.size(),	current_Mesh->m_vertices.data(),	GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,	sizeof(unsigned int)*	current_Mesh->m_indices.size(),		current_Mesh->m_indices.data(),		GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // Position.
		glEnableVertexAttribArray(1); // Tex Coord.
		glEnableVertexAttribArray(2); // Bone Indices.
		glEnableVertexAttribArray(3); // Bone Weights.
		glEnableVertexAttribArray(4); // Normals.

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	// -----------------------------------
}

void Assignment::EvaluateSkeleton(FBXAnimation* _animation, FBXSkeleton* _skeletion, float _timer)
{
	/*m_fps				= 20.f;*/
	int current_Frame	= (int)(_timer * m_fps);

	// Loop through all the bones.
	for (unsigned int track_Index = 0; track_Index < _animation->m_trackCount; track_Index++)
	{
		// Wrap back to the start of the track if we've gone too far.
		int track_Frame_Count	= _animation->m_tracks[track_Index].m_keyframeCount;
		int track_Frame			= current_Frame % track_Frame_Count;

		// Find what key frames are currently effecting the bones.
		FBXKeyFrame curr_Frame = _animation->m_tracks[track_Index].m_keyframes[track_Frame];
		FBXKeyFrame next_Frame = _animation->m_tracks[track_Index].m_keyframes[(track_Frame + 1) % track_Frame_Count];

		// Find out how far between key frames we are.
		float TSFF	= _timer - (current_Frame / m_fps); // TSFF = Time Since Frame Flip.
		float t		= TSFF * m_fps;

		// Interpolate between those key frames to generate the matrix for the current pose.
		vec3 new_Pos			= mix(curr_Frame.m_translation, next_Frame.m_translation, t);
		vec3 new_Scale			= mix(curr_Frame.m_scale, next_Frame.m_scale, t);

		quat new_Rot			= slerp(curr_Frame.m_rotation, next_Frame.m_rotation, t);

		mat4 local_Transform	= translate(new_Pos) * toMat4(new_Rot) * scale(new_Scale);

		// Get the right bone for the given track.
		int bone_Index = _animation->m_tracks[track_Index].m_boneIndex;

		// Set the FBXNode's local trancsform to match.
		if (bone_Index < _skeletion->m_boneCount)
		{
			_skeletion->m_nodes[bone_Index]->m_localTransform = local_Transform;
		}
	}
}

void Assignment::UpdateBones(FBXSkeleton* _skeleton)
{
	// Loop though the nodes in the skeleton.
	for (unsigned int bone_Index = 0; bone_Index < _skeleton->m_boneCount; bone_Index++)
	{
		// Generate theie global transform.
		int parent_Index = _skeleton->m_parentIndex[bone_Index];

		if (parent_Index == -1)
		{
			_skeleton->m_bones[bone_Index] = _skeleton->m_nodes[bone_Index]->m_localTransform;
		}
		else
		{
			_skeleton->m_bones[bone_Index] = _skeleton->m_bones[parent_Index] *_skeleton->m_nodes[bone_Index]->m_localTransform;
		}
	}

	for (unsigned int bone_Index = 0; bone_Index < _skeleton->m_boneCount; bone_Index++)
	{
		// Multiply the global transform transform by the invser bind pose
		_skeleton->m_bones[bone_Index] = _skeleton->m_bones[bone_Index] * _skeleton->m_bindPoses[bone_Index];
	}
}
// ----------------------------------------------------------------------------

// AntTweakBar stuff.
void Assignment::AntTweakButtons()
{
	m_bar = TwNewBar("GUI");

	// GUI for the terrian.
	TwAddVarRW(m_bar, "Real Dims",			TW_TYPE_FLOAT, &m_real_Dims,	"group=Terrian_Settings");
	TwAddVarRW(m_bar, "Dims",				TW_TYPE_FLOAT, &m_dims,			"group=Terrian_Settings");
	TwAddVarRW(m_bar, "Octaves",			TW_TYPE_INT32, &m_octaves,		"group=Terrian_Settings");
	TwAddVarRW(m_bar, "Persistance",		TW_TYPE_FLOAT, &m_persistance,	"group=Terrian_Settings");
	TwAddVarRW(m_bar, "Jitter",				TW_TYPE_FLOAT, &m_scale,		"group=Terrian_Settings");
	// -----------------------------------

	// GUI for the animated mesh.
	/*TwAddVarRW(m_bar, "Position", TW_TYPE_FLOAT, &m_mesh_Position, "group=Animated Mesh");*/
	TwAddVarRW(m_bar, "Animation_FPS",		TW_TYPE_FLOAT, &m_fps,			"group=Animated_Mesh");
	/*TwAddVarRW(m_bar, "Animation_Frame", TW_TYPE_INT32, &m_current_Frame, "group=Animated_Mesh");*/
	// -----------------------------------
	
	// GUI for the lighting.
	TwAddVarRW(m_bar, "Light_Direction",	TW_TYPE_DIR3F, &light_Dir,		"group=Lighting");
	TwAddVarRW(m_bar, "Light_Color",		TW_TYPE_COLOR3F, &light_Color,	"group=Lighting");

	TwAddVarRW(m_bar, "Reload",				TW_TYPE_BOOL8, &m_reload,		"");
}
// ----------------------------------------------------------------------------

void Assignment::LoadTexture()
{
	int image_Width = 0, image_Height = 0, image_Format = 0;
	unsigned char* data;

	data = stbi_load("./textures/rock_diffuse.tga", &image_Width, &image_Height, &image_Format, STBI_default);

	glGenTextures(1, &m_terrian_Texture);
	glBindTexture(GL_TEXTURE_2D, m_terrian_Texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_Width, image_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	/*data = stbi_load("./textures/rock_normal.tga", &image_Width, &image_Height, &image_Format, STBI_default);

	glGenTextures(1, &m_terrian_Normal);
	glBindTexture(GL_TEXTURE_2D, m_terrian_Normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_Width, image_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);*/
}

void Assignment::ReloadShader()
{
	LoadShader("./Shaders/Perlin_Vertex.glsl", 0, "./Shaders/Perlin_Fragment.glsl", &m_perlin_Program_ID);
	LoadShader("./Shaders/Skinned_Vertex.glsl", 0, "./Shaders/Skinned_Fragment.glsl", &m_fbx_Program_ID);
	LoadShader("./Shaders/Skinned_Vertex.glsl", 0, "./Shaders/Skinned_Fragment.glsl", &m_fbx_Program_ID_2);
}