#include "Emitter.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>

Emitter::Emitter() : m_particles(nullptr), m_max_Particles(0), m_particle_Count(0), m_vertex_Data(nullptr), m_index_Data(nullptr)
{
	m_buffers = {};
}

Emitter::~Emitter()
{
	delete[] m_particles;
	delete[] m_vertex_Data;
	delete[] m_index_Data;

	glDeleteVertexArrays(1, &m_buffers.m_VAO);
	glDeleteBuffers(1, &m_buffers.m_VBO);
	glDeleteBuffers(1, &m_buffers.m_IBO);
}

void Emitter::Init(unsigned int _max_Particles, vec3 _position, float _emit_Rate,
	float _min_Lifespan, float _max_Lifespan, float _min_Velocity, float _max_Velocity,
	float _start_Size, float _end_Size, vec4 _start_Color, vec4 _end_Color)
{
	m_position			= vec4(_position, 1);
	m_lifespan_Min		= _min_Lifespan;
	m_lifespan_Max		= _max_Lifespan;
	m_velocity_Min		= _min_Velocity;
	m_velocity_Max		= _max_Velocity;
	m_start_Size		= _start_Size;
	m_end_Size			= _end_Size;
	m_start_Color		= _start_Color;
	m_end_Color			= _end_Color;

	m_emit_Rate			= 1.0f / _emit_Rate;

	m_max_Particles		= _max_Particles;
	m_particle_Count	= 0;

	m_particles			= new Particles[m_max_Particles];
	m_vertex_Data		= new VertexParticles[m_max_Particles * 4];
	m_index_Data		= new unsigned int[m_max_Particles * 6];

	for (unsigned int i = 0; i < m_max_Particles; i++)
	{
		unsigned int start = 4 * i;

		m_index_Data[i * 6 + 0] = start + 0; 
		m_index_Data[i * 6 + 1] = start + 1;
		m_index_Data[i * 6 + 2] = start + 2;
						   
		m_index_Data[i * 6 + 3] = start + 0;
		m_index_Data[i * 6 + 4] = start + 2;
		m_index_Data[i * 6 + 5] = start + 3;
	}

	glGenBuffers(1, &m_buffers.m_VBO);
	glGenBuffers(1, &m_buffers.m_IBO);
	glGenVertexArrays(1, &m_buffers.m_VAO);

	glBindVertexArray(m_buffers.m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_max_Particles * 4 * sizeof(VertexParticles), m_vertex_Data, GL_DYNAMIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffers.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_max_Particles * 6 * sizeof(unsigned int), m_index_Data, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Position.
	glEnableVertexAttribArray(1); // Color.

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticles), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexParticles), (void*)sizeof(vec4));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Emitter::EmitParticles()
{
	unsigned int particles_To_Emit = unsigned int(m_emit_Timer / m_emit_Rate);
	m_emit_Timer -= particles_To_Emit * m_emit_Rate;

	for (unsigned int i = 0; i < particles_To_Emit && m_particle_Count < m_max_Particles; i++)
	{
		m_particles[m_particle_Count].position = m_position;
		m_particles[m_particle_Count].lifetime = 0;
		m_particles[m_particle_Count].lifespan = linearRand(m_lifespan_Min, m_lifespan_Max);

		m_particles[m_particle_Count].color = vec4(((float)(rand() % 255) / 255.f), ((float)(rand() % 255) / 255.f), ((float)(rand() % 255) / 255.f), 1);
		m_particles[m_particle_Count].size = m_start_Size;

		float velocity_Length = linearRand(m_velocity_Min, m_velocity_Max);

		m_particles[m_particle_Count].velocity.xyz = sphericalRand(velocity_Length);
		m_particles[m_particle_Count].velocity.w = 0;

		++m_particle_Count;
	}
}

void Emitter::Update(float _dt) // Simulating particles function.
{
	// Clean up dead particles.
	for (unsigned int i = 0; i < m_particle_Count; i++)
	{
		m_particles[i].lifetime += _dt;

		if (m_particles[i].lifetime > m_particles[i].lifespan)
		{
			--m_particle_Count;
			m_particles[i] = m_particles[m_particle_Count];
			i--;
		}
	}

	// Spawn new particles.
	m_emit_Timer += _dt;
	EmitParticles();

	// Move all alive particles.
	for (unsigned int i = 0; i < m_particle_Count; i++)
	{
		m_particles[i].position += _dt * m_particles[i].velocity;

		float t = m_particles[i].lifetime / m_particles[i].lifespan;

		m_particles[i].color = mix(vec4(((float)(rand() % 255) / 255.f), ((float)(rand() % 255) / 255.f), ((float)(rand() % 255) / 255.f), 1), m_end_Color, t);
		m_particles[i].size = mix(m_start_Size, m_end_Size, t);
	}
}

void Emitter::UpdateVertexData(vec3 _cam_Pos, vec3 _cam_Up)
{
	for (unsigned int i = 0; i < m_particle_Count; i++)
	{
		mat4 particle_Transform;

		vec3 to = _cam_Pos - m_particles[i].position.xyz;
		vec3 f = normalize(to);
		vec3 r = cross(_cam_Up, f);
		vec3 u = cross(f, r);

		f *= m_particles[i].size;
		r *= m_particles[i].size;
		u *= m_particles[i].size;

		particle_Transform[0].xyz = r;
		particle_Transform[1].xyz = u;
		particle_Transform[2].xyz = f;
		particle_Transform[3] = m_particles[i].position;

		m_vertex_Data[i * 4 + 0].position = particle_Transform * vec4(-1, 1, 0, 1);
		m_vertex_Data[i * 4 + 1].position = particle_Transform * vec4(-1, -1, 0, 1);
		m_vertex_Data[i * 4 + 2].position = particle_Transform * vec4(1, -1, 0, 1);
		m_vertex_Data[i * 4 + 3].position = particle_Transform * vec4(1, 1, 0, 1);

		m_vertex_Data[i * 4 + 0].color = m_particles[i].color;
		m_vertex_Data[i * 4 + 1].color = m_particles[i].color;
		m_vertex_Data[i * 4 + 2].color = m_particles[i].color;
		m_vertex_Data[i * 4 + 3].color = m_particles[i].color;

	}
}

void Emitter::Render()
{
	glBindBuffer(GL_ARRAY_BUFFER, m_buffers.m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_particle_Count * 4 * sizeof(VertexParticles), m_vertex_Data);
	
	glBindVertexArray(m_buffers.m_VAO);
	glDrawElements(GL_TRIANGLES, 6 * m_particle_Count, GL_UNSIGNED_INT, 0);
}