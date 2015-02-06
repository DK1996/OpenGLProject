#ifndef _INTRO_OGL_H_
#define _INTRO_OGL_H_

#include "Application.h"

class IntroToOpenGL : public Application
{
public:
	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update();
	virtual void Draw();

	mat4 m_view;
	mat4 m_projection;

	float m_camera_x;
	float m_camera_z;

	float m_timer;

private:

};

#endif