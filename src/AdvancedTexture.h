#ifndef ADV_TEX_H
#define ADV_TEX_H

#include "Application.h"
#include "Camera.h"
#include "glm_header.h"
#include "Vertex.h"

class AdvancedTexture : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void GenQuad(float size);
	void LoadTexture();

	OpenGLData m_quad;

	unsigned int m_program_ID;
	unsigned int m_diffuse_Texture;
	unsigned int m_normal_Texture;
	unsigned int m_spec_Texture;

	vec3 m_light_Dir;
	vec3 m_light_Color;
	vec3 m_ambient_Light;

	float m_spec_Power;

	Camera* m_camera;

	float dt;


};

#endif
