#include "AdvancedNavigation.h"
#include "gl_core_4_4.h"
#include "FlyCamera.h"
#include "Gizmos.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"
#include "tiny_obj_loader.h"

bool AdvancedNavigation::StartUp()
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

	m_sponza_Mesh = LoadOBJ("./Models/SponzaSimple.obj");

	LoadShader("./Shaders/Nav_Mesh_Vertex.glsl", 0, "./Shaders/Nav_Mesh_Fragment.glsl", &m_program_ID);

	BuildNavMesh();

	return true;
}

void AdvancedNavigation::ShutDown()
{
	Gizmos::destroy();

	Application::ShutDown();
}

bool AdvancedNavigation::Update()
{
	if (Application::Update() == false)
	{
		return false;
	}

	if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS)
	{
		ReloadShader();
	}

	for (unsigned int node_Index = 0; node_Index < m_nav_Nodes.size(); node_Index++)
	{
		vec3 raise = vec3(0, 0.1f, 0);
		Gizmos::addAABBFilled(m_nav_Nodes[node_Index].corners[0] + raise, vec3(0.1), vec4(0.7f, 0, 1, 1));
		Gizmos::addAABBFilled(m_nav_Nodes[node_Index].corners[1] + raise, vec3(0.1), vec4(0.7f, 0, 1, 1));
		Gizmos::addAABBFilled(m_nav_Nodes[node_Index].corners[2] + raise, vec3(0.1), vec4(0.7f, 0, 1, 1));

		Gizmos::addAABBFilled(m_nav_Nodes[node_Index].position + raise, vec3(0.1), vec4(0, 0, 1, 1));

		Gizmos::addLine(m_nav_Nodes[node_Index].corners[0] + raise, m_nav_Nodes[node_Index].corners[1] + raise, vec4(0, 1, 0, 1));
		Gizmos::addLine(m_nav_Nodes[node_Index].corners[1] + raise, m_nav_Nodes[node_Index].corners[2] + raise, vec4(0, 1, 0, 1));
		Gizmos::addLine(m_nav_Nodes[node_Index].corners[2] + raise, m_nav_Nodes[node_Index].corners[0] + raise, vec4(0, 1, 0, 1));

		for (unsigned int edge = 0; edge < 3; edge++)
		{
			if (m_nav_Nodes[node_Index].edges[edge] != 0)
			{
				Gizmos::addLine(m_nav_Nodes[node_Index].position + raise, m_nav_Nodes[node_Index].edges[0]->position + raise, vec4(1, 1, 0, 1));
			}
		}
	}

	m_dt = glfwGetTime();
	glfwSetTime(0.0);
	m_camera->Update(m_dt);

	return true;
}

void AdvancedNavigation::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Gizmos::draw(m_camera->m_projectionView);

	glUseProgram(m_program_ID);

	int proj_View_Uni = glGetUniformLocation(m_program_ID, "proj_View");

	glUniformMatrix4fv(proj_View_Uni, 1, GL_FALSE, (float*)&m_camera->m_projectionView);

	glBindVertexArray(m_sponza_Mesh.m_VAO);
	glDrawElements(GL_TRIANGLES, m_sponza_Mesh.m_index_Count, GL_UNSIGNED_INT, 0);

	glfwSwapBuffers(m_window);
	glfwPollEvents();
}

void AdvancedNavigation::BuildNavMesh()
{
	using namespace tinyobj;

	// Load our nav mesh from the obj file.
	vector<shape_t> shapes;
	vector<material_t> materials;

	string err = LoadObj(shapes, materials, "./Models/SponzaSimpleNavMesh.obj");

	if (err.size() != 0)
	{
		printf("%s\n", err.c_str());
		return;
	}

	if (shapes.size() != 1)
	{
		printf("obj did not have 1 mesh in it\n");
		return;
	}

	mesh_t* mesh = &shapes[0].mesh;
	
	// Resize our nav_Nodes vector to hold the right number of nodes.
	unsigned int index_Count = mesh->indices.size();
	m_nav_Nodes.resize(index_Count / 3);

	// Loop through all the indices and build our nodes.
	// Set the centers and corners.
	for (unsigned int index = 0; index < index_Count; index += 3)
	{
		unsigned int node_Index = index / 3;

		for (unsigned int corner_Index = 0; corner_Index < 3; corner_Index++)
		{
			unsigned int vertex_Index = mesh->indices[index + corner_Index];

			m_nav_Nodes[node_Index].corners[corner_Index].x = mesh->positions[vertex_Index * 3 + 0];
			m_nav_Nodes[node_Index].corners[corner_Index].y = mesh->positions[vertex_Index * 3 + 1];
			m_nav_Nodes[node_Index].corners[corner_Index].z = mesh->positions[vertex_Index * 3 + 2];
		}

		m_nav_Nodes[node_Index].position = (m_nav_Nodes[node_Index].corners[0] + m_nav_Nodes[node_Index].corners[1] + m_nav_Nodes[node_Index].corners[2]) / 3.0f;

	}

	// Loop through all our nodes.
		// Loop through all of them again.
		// Look for matching edges.
	for (auto& first : m_nav_Nodes)
	{
		unsigned int curr_Edge = 0;

		first.edges[0] = 0;
		first.edges[1] = 0;
		first.edges[2] = 0;

		for (auto& second : m_nav_Nodes)
		{
			if (&first == &second)
			{
				continue;
			}

			for (unsigned int EI = 0; EI < 3; EI++)
			{
				if ( first.corners[EI] == second.corners[0] && first.corners[(EI + 1) % 3] == second.corners[1] || 
					 first.corners[EI] == second.corners[1] && first.corners[(EI + 1) % 3] == second.corners[2] || 
					 first.corners[EI] == second.corners[2] && first.corners[(EI + 1) % 3] == second.corners[0] ||
					 
					 first.corners[EI] == second.corners[1] && first.corners[(EI + 1) % 3] == second.corners[0] ||
					 first.corners[EI] == second.corners[2] && first.corners[(EI + 1) % 3] == second.corners[1] ||
					 first.corners[EI] == second.corners[0] && first.corners[(EI + 1) % 3] == second.corners[2]  )
				{
					// Make an edge from first to second.
					first.edges[curr_Edge] = &second;
					first.edge_Costs[curr_Edge] = glm::distance(second.position, first.position);
					curr_Edge++;
					break;
				}
			}

			if (curr_Edge == 3)
			{
				break;
			}
		}
	}
}

void AdvancedNavigation::ReloadShader()
{
	LoadShader("./Shaders/Nav_Mesh_Vertex.glsl", 0, "./Shaders/Nav_Mesh_Fragment.glsl", &m_program_ID);
}