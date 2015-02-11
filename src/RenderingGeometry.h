#ifndef RENDERING_GEO_H
#define RENDERING_GEO_H

#include "Application.h"
#include "Vertex.h"

class FlyCamera;

class RenderingGeometry : public Application
{
public:

	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update();
	virtual void Draw();

	void GenerateGrid(unsigned int rows, unsigned int cols);
	void GenerateShader();

	unsigned int m_program_id;
	unsigned int m_index_count;


	FlyCamera *m_camera;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	float dt;

private:

};

#endif