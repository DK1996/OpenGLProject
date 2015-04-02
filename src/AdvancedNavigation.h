#ifndef ADV_NAV_H
#define ADV_NAV_H

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

#include <vector>

using namespace std;

struct NavMeshNode
{
	// Center point of the polygon.
	vec3 position;
	vec3 corners[3];
	NavMeshNode* edges[3];

	// A* data.
	unsigned int flags;
	float edge_Costs[3];
};

class AdvancedNavigation : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void BuildNavMesh();

	void ReloadShader();

	// Program for the sponza shader.
	unsigned int m_program_ID;

	// Store sponza mesh we're rendering.
	OpenGLData m_sponza_Mesh;

	// NavMesh nodes.
	vector<NavMeshNode> m_nav_Nodes;


private:

	Camera*	m_camera;

	float	m_dt;

};

#endif // !ADV_NAV_H