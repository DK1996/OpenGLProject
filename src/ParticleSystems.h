#ifndef PARTICLE_SYSTEMS_H
#define PARTICLE_SYSTEMS_H

#include "Application.h"
#include "Camera.h"
#include "Emitter.h"

class ParticleSystems : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	unsigned int m_program_ID;

	float m_dt;

	Camera* m_camera;

	Emitter m_emitter;

};

#endif