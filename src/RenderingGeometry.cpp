#include "RenderingGeometry.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "glm_header.h"
#include "Gizmos.h"
#include "Uitility.h"
#include "Vertex.h"

bool RenderingGeometry::StartUp()
{
	if (Application::StartUp() == false)
	{
		return false;
	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	Gizmos::create();

	LoadShader("./Shaders/Basic_Vertex.glsl", nullptr, "./Shaders/basic_fragment.glsl", &m_program_id);
	GenerateGrid(10, 10);
	
	m_camera = new FlyCamera();
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0, 0, 0), vec3(0, 1, 0));
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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glUseProgram(m_program_id);

	int proj_view_handle = glGetUniformLocation(m_program_id, "projection_view");

	if (proj_view_handle > -1)
	{
		glUniformMatrix4fv(proj_view_handle, 1, false, (float*)&m_camera->m_projectionView);
	}

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void RenderingGeometry::GenerateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* vertex_array = new Vertex[(rows + 1) * (cols + 1)];

	for (unsigned int r = 0; r < rows + 1; r++)
	{
		for (unsigned int c = 0; c < cols + 1; c++)
		{
			vec4 pos	= vec4((float)c, 0, (float)r, 1);
			vec4 color	= vec4(1, 1, 1, 1);

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
			index_array[index_location + 2] = (c + 1) + (r + 1) * (cols + 1);

			index_array[index_location + 3] = c + r * (cols + 1);
			index_array[index_location + 4] = (c + 1) + r * (cols + 1);
			index_array[index_location + 5] = (c + 1) + (r + 1) * (cols + 1);

			index_location += 6;
		}
	}

	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	glGenVertexArrays(1, &m_VAO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, ((cols + 1) * (rows + 1)) * sizeof(Vertex), vertex_array, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Color.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_location * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] vertex_array;
	delete[] index_array;
}