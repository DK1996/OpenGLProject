#ifndef EMITTER_H
#define EMITTER_H

#include "glm_header.h"
#include "Vertex.h"

struct Particles
{
	vec4 velocity;
	vec4 position;
	vec4 color;

	float size;
	float lifetime;
	float lifespan;
};


// What does an emitter do?
// It contains all of the particles it controls.
// It spawns new particles. 
// It updates existing particles.
// It renders its particles.
class Emitter
{
public:
	Emitter();
	~Emitter();

	void Init(unsigned int _max_Particles, vec3 _position, float _emit_Rate,
			  float _min_Lifespan, float _max_Lifespan, float _min_Velocity, float _max_Velocity, 
			  float _start_Size, float _end_Size, vec4 _start_Color, vec4 _end_Color);

	void EmitParticles();
	void Update(float _dt); // Simulating particles function.
	void Render();

	void UpdateVertexData(vec3 _cam_Pos, vec3 _cam_Up);

	// Particle Data.
	Particles* m_particles;

	unsigned int m_max_Particles;
	unsigned int m_particle_Count; // Particles that are alive.

	// OpenGL Data.
	OpenGLData m_buffers;

	VertexParticles* m_vertex_Data;
	unsigned int* m_index_Data;

	// Emitter Data.
	vec4 m_position;

	float m_emit_Rate;
	float m_emit_Timer;

	float m_lifespan_Min;
	float m_lifespan_Max;
	float m_velocity_Min;
	float m_velocity_Max;

	float m_start_Size;
	float m_end_Size;

	vec4 m_start_Color;
	vec4 m_end_Color;

};

#endif // !EMITTER_H
