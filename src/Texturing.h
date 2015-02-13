#ifndef TEX_H
#define TEX_H

#include "Application.h"
#include "Camera.h"

class Texturing : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void LoadTexture(const char* filename);
	void GenerateQuad(float size);

	unsigned int m_program_ID;

	Camera* m_camera;

	unsigned int m_texture;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	float dt;

};

#endif