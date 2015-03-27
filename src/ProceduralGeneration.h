#ifndef PROCEDURAL_GEN_H
#define PROCEDURAL_GEN_H

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

class ProceduralGeneration : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void BuildGrid(vec2 _real_Dims, ivec2 _dims);
	void BuildPerlinTexture(ivec2 _dims, int _octaves, float _persistance);

	void ReloadShader();

	OpenGLData		m_plane_Mesh;

	unsigned int	m_program_ID;
	unsigned int	m_perlin_Texture;

	float*			m_perlin_Data;
	float			m_time;

private:

	Camera*			m_camera;

	float			m_dt;

};

#endif // !PROCEDURAL_GEN_H