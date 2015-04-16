#ifndef ASSIGNMENT_H_
#define ASSIGNMENT_H_

#include "gl_core_4_4.h"

#include "Application.h"
#include "Camera.h"
#include "FlyCamera.h"
#include "Gizmos.h"

#include "AntTweakBar.h"
#include "tiny_obj_loader.h"

#include "Vertex.h"
#include "Uitility.h"

#include <GLFW/glfw3.h>

class Assignment : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	// ProceduralGeneration
	void BuildGrid(vec2 _real_Dims, ivec2 _dims);
	void BuildPerlinTexture(ivec2 _dims, int _octaves, float _persistance);

	OpenGLData		m_plane_Mesh;

	unsigned int	m_perlin_Program_ID;
	unsigned int	m_perlin_Texture;
	unsigned int	m_terrian_Texture;
	unsigned int	m_terrian_Normal;

	float*			m_perlin_Data;

	float			m_real_Dims;
	float			m_dims;

	int				m_octaves;
	float			m_persistance;

	float			m_amplitute;
	float			m_freq;

	float			m_scale;

	bool			m_reload;
	// -----------------------------------

	void LoadTexture();

	void ReloadShader();

	TwBar*		m_bar;

	FlyCamera*	m_speed;

private:

	Camera*	m_camera;

	float	m_dt;

};

#endif // !ASSIGNMENT_H_