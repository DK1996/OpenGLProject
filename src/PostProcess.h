#ifndef POST_PORCESS_H
#define POST_PORCESS_H

#include "Application.h"
#include "Camera.h"
#include "Vertex.h"

class PostProcess : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	void BuildFrameBuffer();
	void BuildQuad();

	unsigned int m_FBO;
	unsigned int m_FBO_Texture;
	unsigned int m_FBO_depth;

	unsigned int m_post_Program;

	OpenGLData m_quad;

private:

	Camera* m_camera;

	float dt;

};

#endif // !POST_PORCESS_H