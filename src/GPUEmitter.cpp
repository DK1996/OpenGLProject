#include "GPUEmitter.h"
#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include "Uitility.h"

GPUPointEmitter::GPUPointEmitter() : m_particles(nullptr), m_max_Particles(0), m_position(0, 0, 0), m_draw_Shader(0), m_update_Shader(0), m_last_Draw_Time(0)
{
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}

GPUPointEmitter::~GPUPointEmitter()
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	// Delete the shaders.
	glDeleteProgram(m_draw_Shader);
	glDeleteProgram(m_update_Shader);
}

void GPUPointEmitter::Init(unsigned int _max_Particles, vec3 _position, float _emit_Rate,
						   float _min_Lifespan, float _max_Lifespan, float _min_Velocity, float _max_Velocity,
						   float _start_Size, float _end_Size, vec4 _start_Color, vec4 _end_Color)
{
	m_position		= (_position);
	m_lifespan_Min	= _min_Lifespan;
	m_lifespan_Max	= _max_Lifespan;
	m_velocity_Min	= _min_Velocity;
	m_velocity_Max	= _max_Velocity;
	m_start_Size	= _start_Size;
	m_end_Size		= _end_Size;
	m_start_Color	= _start_Color;
	m_end_Color		= _end_Color;

	m_max_Particles = _max_Particles;

	// Create particle array.
	m_particles = new GPUParticle[_max_Particles];

	m_actice_Buffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void GPUPointEmitter::Draw(float _curr_Time, mat4 _camera_Transform, mat4 _projection_View)
{
	// Update vertex pass.
	glUseProgram(m_update_Shader);

	int delta_Time_Uniform			= glGetUniformLocation(m_update_Shader, "delat_Time");
	int emitter_Position_Uniform	= glGetUniformLocation(m_update_Shader, "emitter_Position");
	int min_Velocity_Uniform		= glGetUniformLocation(m_update_Shader, "min_Velocity");
	int max_Velocity_Uniform		= glGetUniformLocation(m_update_Shader, "max_Velocity");
	int min_Lifespan_Uniform		= glGetUniformLocation(m_update_Shader, "min_Lifespan");
	int max_Lifespan_Uniform		= glGetUniformLocation(m_update_Shader, "max_Lifespan");
	int time_Uniform				= glGetUniformLocation(m_update_Shader, "time");

	glUniform1f(delta_Time_Uniform, _curr_Time - m_last_Draw_Time);
	glUniform3fv(emitter_Position_Uniform, 1, (float*)&m_position);
	glUniform1f(min_Velocity_Uniform, m_velocity_Min);
	glUniform1f(max_Velocity_Uniform, m_velocity_Max);
	glUniform1f(min_Lifespan_Uniform, m_lifespan_Min);
	glUniform1f(max_Lifespan_Uniform, m_lifespan_Max);
	glUniform1f(time_Uniform, _curr_Time);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vao[m_actice_Buffer]);

	unsigned int other_Buffer = (m_actice_Buffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[other_Buffer]);

	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_max_Particles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK, 0, 0);

	// Render pass.
	glUseProgram(m_draw_Shader);

	int projection_View_Uniform	= glGetUniformLocation(m_draw_Shader, "projection_View");
	int camera_World_Uniform	= glGetUniformLocation(m_draw_Shader, "camera_World");
	int start_Size_Uniform		= glGetUniformLocation(m_draw_Shader, "start_Size");
	int end_Size_Uniform		= glGetUniformLocation(m_draw_Shader, "end_Size");
	int start_Color_Uniform		= glGetUniformLocation(m_draw_Shader, "start_Color");
	int end_Color_Uniform		= glGetUniformLocation(m_draw_Shader, "end_Color");

	glUniformMatrix4fv(projection_View_Uniform, 1, GL_FALSE, (float*)&_projection_View);
	glUniformMatrix4fv(camera_World_Uniform, 1, GL_FALSE, (float*)&_camera_Transform);
	glUniform1f(start_Size_Uniform, m_start_Size);
	glUniform1f(end_Size_Uniform, m_end_Size);
	glUniform4fv(start_Color_Uniform, 1, (float*)&m_start_Color);
	glUniform4fv(start_Color_Uniform, 1, (float*)&m_end_Color);

	glBindVertexArray(m_vao[other_Buffer]);
	glDrawArrays(GL_POINTS, 0, m_max_Particles);

	m_actice_Buffer = other_Buffer;

	m_last_Draw_Time = _curr_Time;
}

void GPUPointEmitter::CreateBuffers()
{
	// Create OpenGL buffers.
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	// Setting you the buffers in a for loop.
	for (unsigned int buffer_Index = 0; buffer_Index < 2; buffer_Index++)
	{
		glBindVertexArray(m_vao[buffer_Index]);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[buffer_Index]);
		glBufferData(GL_ARRAY_BUFFER, m_max_Particles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);
		
		glEnableVertexAttribArray(0); // Position.
		glEnableVertexAttribArray(1); // Velocity.
		glEnableVertexAttribArray(2); // Lifetime.
		glEnableVertexAttribArray(3); // Lifespan.

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 12);
		glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 24);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), ((char*)0) + 28);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void GPUPointEmitter::CreateUpdateShader()
{
	// Create shader.
	unsigned int vs;
	LoadShaderType("./Shaders/Particle_Update_Vert.glsl", GL_VERTEX_SHADER, &vs);

	m_update_Shader = glCreateProgram();
	glAttachShader(m_update_Shader, vs);

	const char* outputs[4] = { "updated_Position", "updated_Velocity", "updated_Lifespan", "updated_Lifetime" };

	glTransformFeedbackVaryings(m_update_Shader, 4, 0, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_update_Shader);
	glDeleteShader(vs);
}

void GPUPointEmitter::CreateDrawShader()
{
	LoadShader("./Shader/GPU_Particle_Vert.glsl", "./Shaders/GPU_Particle_Geom.glsl", "./Shaders/GPU_Particle_Frag", &m_draw_Shader);
}