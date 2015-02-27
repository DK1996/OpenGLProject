#ifndef GPU_EMITTER_H
#define GPU_EMITTER_H

#include "glm_header.h"
#include "Vertex.h"

// GPU particle struct.
struct GPUParticle
{
	GPUParticle() : lifespan(0), lifetime(1) {}

	vec3 position;
	vec3 velocity;

	float lifetime;
	float lifespan;
};

// GPU emitter class.
	// GPU buffers.
	// Transform feedback.
	// Creating shaders.
	// Rendering them.
class GPUPointEmitter
{
public:
	GPUPointEmitter();
	~GPUPointEmitter();

	void Init(unsigned int _max_Particles, vec3 _position, float _emit_Rate,
			  float _min_Lifespan, float _max_Lifespan, float _min_Velocity, float _max_Velocity,
			  float _start_Size, float _end_Size, vec4 _start_Color, vec4 _end_Color);

	void Draw(float _curr_Time, mat4 _camera_Transform, mat4 _projection_View);

	void CreateBuffers();
	void CreateUpdateShader();
	void CreateDrawShader();

	GPUParticle*		m_particles;

	unsigned int		m_max_Particles;
	unsigned int		m_particle_Count; // Particles that are alive.

	// OpenGL Data.
	OpenGLData			m_buffers;

	VertexParticles*	m_vertex_Data;
	unsigned int*		m_index_Data;

	// Emitter Data.
	vec3				m_position;

	float				m_emit_Rate;
	float				m_emit_Timer;

	float				m_lifespan_Min;
	float				m_lifespan_Max;
	float				m_velocity_Min;
	float				m_velocity_Max;

	float				m_start_Size;
	float				m_end_Size;

	vec4				m_start_Color;
	vec4				m_end_Color;

	unsigned int		m_actice_Buffer;
	unsigned int		m_vao[2];
	unsigned int		m_vbo[2];

	unsigned int		m_draw_Shader;
	unsigned int		m_update_Shader;

	float				m_last_Draw_Time;

};

#endif // !GPU_EMITTER_H