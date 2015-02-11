#include "RenderingGeometry.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "glm_header.h"
#include "Gizmos.h"
#include "Uitility.h"

bool RenderingGeometry::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	GenerateShader();
	GenerateGrid(10, 10);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Gizmos::create();

	m_camera = new FlyCamera();
	m_camera->setPerspective(pi<float>() *0.25, 1280.f / 720.f, 0.1f, 1000.f);

	return true;
}

void RenderingGeometry::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool RenderingGeometry::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	dt = (float)glfwGetTime();
	glfwSetTime(0.0);

	m_camera->Update(dt);

	glClearColor(0.3f, 0.3f, 0.3f, 1.f);
	glEnable(GL_DEPTH_TEST);

	return true;
}

void RenderingGeometry::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_camera->Update(dt);
	Gizmos::draw(m_camera->m_projectionView);

	glUseProgram(m_program_id);

	int proj_view_handle = glGetUniformLocation(m_program_id, "projection_view");

	if (proj_view_handle >= 0)
	{
		glUniformMatrix4fv(proj_view_handle, 1, false, (float*)&m_camera->m_projectionView);
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();

	Application::Draw();
}

void RenderingGeometry::GenerateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* vertex_array = new Vertex[(rows + 1) * (cols + 1)];

	for (unsigned int r = 0; r < rows + 1; r++)
	{
		for (unsigned int c = 0; c < cols; c++)
		{
			vec4 pos	= vec4((float)c, 0, (float)r, 1);
			vec4 color	= vec4((float)c / cols, 0, (float)r / rows, 1);

			vertex_array[c + r * (cols + 1)].position = pos;
			vertex_array[c + r * (cols + 1)].color = color;
		}
	}

	m_index_count = rows * cols * 6;

	unsigned int *index_array = new unsigned int[m_index_count];

	int index_location = 0;

	for (unsigned int r = 0; r < rows; r++)
	{
		for (unsigned int c = 0; c < cols; c++)
		{
			index_array[index_location + 0] = c + r * (cols + 1);
			index_array[index_location + 1] = c + (r + 1) * (cols + 1);
			index_array[index_location + 2] = (c + 1) + r * (cols + 1);

			index_array[index_location + 3] = (c + 1) + r * (cols + 1);
			index_array[index_location + 4] = c + (r + 1) * (cols + 1);
			index_array[index_location + 5] = (c + 1) + (r + 1) * (cols + 1);

			index_location += 6;
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (cols + 1) * (rows + 1) * sizeof(Vertex), vertex_array, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Color.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_index_count * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertex_array;
	delete[] index_array;
}

void RenderingGeometry::GenerateShader()
{
	

	//const char* vs_source = "#version 410\n"
	//						"layout(location=0) in vec4 position;\n"
	//						"layout(location=1) in vec4 color;\n"
	//						"out vec4 out_color;\n"
	//						"uniform mat4 projection_view;\n"
	//						"void main()\n"
	//						"{\n"
	//						"	out_color = color;\n"
	//						"	gl_Position = projection_view * position;\n"
	//						"}\n";

	//const char* fs_source = "#version 410\n"
	//						"in vec4 out_color;\n"
	//						"out vec4 frag_color;\n"
	//						"void main()\n"
	//						"{\n"
	//						"	frag_color = out_color;\n"
	//						"}\n";


	int success = GL_FALSE;
	int log_len = 0;

	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertex_shader, 1, (const char**)&vs_source, 0);
	glCompileShader(vertex_shader);

	glShaderSource(fragment_shader, 1, (const char**)&fs_source, 0);
	glCompileShader(fragment_shader);

	m_program_id = glCreateProgram();
	glAttachShader(m_program_id, vertex_shader);
	glAttachShader(m_program_id, fragment_shader);
	glLinkProgram(vertex_shader);

	// ERROR CHECKING HERE
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_len);

		char* log = new char[log_len];

		glGetShaderInfoLog(vertex_shader, log_len, NULL, log);

		printf("%s\n", log);

		delete[] log;
	}

	if (success == GL_FALSE)
	{
		glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &log_len);

		char* log = new char[log_len];
		glGetProgramInfoLog(m_program_id, log_len, 0, log);

		printf("ERROR: STUFF DONE SCREWED UP IN UR SHADER BUDDY!\n");
		printf("%s", log);

		delete[] log;
	}


	glDeleteShader(fragment_shader);
	glDeleteShader(vertex_shader);

}