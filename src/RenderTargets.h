#ifndef RENDER_TARGETS_H
#define RENDER_TARGETS_H

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

class RenderTargets : public Application
{
public:

	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void GenerateFrameBuffer();
	void GeneratePlane();

	unsigned int m_fbo;
	unsigned int m_fbo_Texture;
	unsigned int m_fbo_Depth;

	unsigned int m_program_ID;

	OpenGLData m_plane;
	
private:

	Camera* m_camera;

	float dt;
};

#endif // !RENDER_TARGETS_H