#ifndef GPU_PARTICLES_H
#define GPU_PARTICLES_H

#include "Application.h"
#include "Camera.h"
#include "GPUEmitter.h"

class GPUParticles : public Application
{
public:
	bool StartUp();
	void ShutDown();
	bool Update();
	void Draw();

	GPUPointEmitter m_emitter;

	Camera* m_camera;

	unsigned int m_programID;

	float m_dt;

	float m_time;

};

#endif