#ifndef VERTEX_H
#define VERTEX_H

struct Vertex
{
	vec4 position;
	vec4 color;
};

struct VertexTexCoord
{
	vec4 position;
	vec2 tex_coord;
};

struct VectexNormal
{
	vec4 position;
	vec4 normal;
	vec4 tangent;
	vec2 tex_Coord;
};

struct OpenGLData
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_Count;
};

#endif