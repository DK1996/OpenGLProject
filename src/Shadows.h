#ifndef SHADOWS_H
#define SHADOWS_H

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

class Shadows : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void BuildMeshes();
	void BuildShadowMaps();

	unsigned int m_FBO;
	unsigned int m_FBO_Depth;

	unsigned int m_shadow_Map_Program;
	unsigned int m_diffuse_Shadowed_Program;

	vec3 m_light_Dir;
	mat4 m_light_Matrix;
	
	OpenGLData m_bunny;
	OpenGLData m_plane;

private:

	Camera* m_camera;

	float dt;

};

#endif // !SHADOWS_H