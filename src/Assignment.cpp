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



	m_real_Dims		= 25.0f;
	m_dims			= 64;
	m_octaves		= 5;
	m_persistance	= 0.3f;

	// Set scale.
	m_scale			= (1.0f / ivec2(m_dims, m_dims).x * 3.0f);

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

	// Loading the shaders here...
	LoadShader("./Shaders/Perlin_Vertex.glsl", 0, "./Shaders/Perlin_Fragment.glsl", &m_perlin_Program_ID);
	// -----------------------------------

	LoadTexture();

	m_bar = TwNewBar("GUI");

	TwAddVarRW(m_bar, "Real Dims",		TW_TYPE_FLOAT, &m_real_Dims,	"group=Terrian Settings");
	TwAddVarRW(m_bar, "Dims",			TW_TYPE_FLOAT, &m_dims,			"group=Terrian Settings");
	TwAddVarRW(m_bar, "Octaves",		TW_TYPE_INT32, &m_octaves,		"group=Terrian Settings");
	TwAddVarRW(m_bar, "Persistance",	TW_TYPE_FLOAT, &m_persistance,	"group=Terrian Settings");
	TwAddVarRW(m_bar, "Jitter",			TW_TYPE_FLOAT, &m_scale,		"group=Terrian Settings");

	TwAddVarRW(m_bar, "Reload", TW_TYPE_BOOL8, &m_reload, "");

	return true;
}

void Assignment::ShutDown()
{
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

	if (m_reload)
	{
		m_reload = !m_reload;
		BuildGrid(vec2(m_real_Dims, m_real_Dims), ivec2(m_dims, m_dims));
		BuildPerlinTexture(ivec2(m_dims, m_dims), m_octaves, m_persistance);
	}

	m_dt = glfwGetTime();
	glfwSetTime(0.0);
	m_camera->Update(m_dt);

	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	return true;
}

void Assignment::Draw()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glUseProgram(m_perlin_Program_ID);

	int view_Proj_Uni = glGetUniformLocation(m_perlin_Program_ID, "view_Proj");
	glUniformMatrix4fv(view_Proj_Uni, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	int tex_Uni = glGetUniformLocation(m_perlin_Program_ID, "perlin_Texture");
	glUniform1i(tex_Uni, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_Texture);

	int terrian_Tex_Uni = glGetUniformLocation(m_perlin_Program_ID, "diffuse");
	glUniform1i(terrian_Tex_Uni, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_terrian_Texture);

	glBindVertexArray(m_plane_Mesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_plane_Mesh.m_index_Count, GL_UNSIGNED_INT, 0);

	TwDraw();

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

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

	data = stbi_load("./textures/rock_normal.tga", &image_Width, &image_Height, &image_Format, STBI_default);

	glGenTextures(1, &m_terrian_Normal);
	glBindTexture(GL_TEXTURE_2D, m_terrian_Normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_Width, image_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);
}

void Assignment::ReloadShader()
{
	LoadShader("./Shaders/Perlin_Vertex.glsl", 0, "./Shaders/Perlin_Fragment.glsl", &m_perlin_Program_ID);
}