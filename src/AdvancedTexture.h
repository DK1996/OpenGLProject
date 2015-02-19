#ifndef ADV_TEX_H
#define ADV_TEX_H

#include "Application.h"
#include "Camera.h"
#include "glm_header.h"
#include "Vertex.h"
#include "AntTweakBar.h"

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

	vec4 m_background_Color;

	float dt;
	float m_spec_Power;

	bool m_draw_Gizmos;
	float m_fps;

	Camera* m_camera;

	TwBar* m_bar;

};

#endif
