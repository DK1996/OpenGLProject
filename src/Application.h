#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "glm_header.h"

struct GLFWwindow;

class Application
{
public:
	Application();
	virtual ~Application();

	virtual bool StartUp();
	virtual void ShutDown();

	virtual bool Update();
	virtual void Draw();

	GLFWwindow* m_window;

private:

};

#endif