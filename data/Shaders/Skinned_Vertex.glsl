#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_coord;
layout(location=2) in vec4 bone_Indices;
layout(location=3) in vec4 bone_Weights;
layout(location=4) in vec4 Normals;

out vec2 frag_tex_coord;
out vec4 frag_Normal;
out vec4 frag_Position;

uniform mat4 projection_view;
uniform mat4 world;

uniform vec3 offset;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main()
{
	frag_tex_coord		= tex_coord;

	frag_Position		= position;

	ivec4 indices		= ivec4(bone_Indices);
	vec4 final_Position = vec4(0, 0, 0, 0);

	final_Position		+= bones[indices.x] * position * bone_Weights.x;
	final_Position		+= bones[indices.y] * position * bone_Weights.y;
	final_Position		+= bones[indices.z] * position * bone_Weights.z;
	final_Position		+= bones[indices.w] * position * bone_Weights.w;

	final_Position.w	= 1;

	final_Position.x	+= offset.x;
	final_Position.y	+= offset.y;
	final_Position.z	+= offset.z;

	vec4 final_Normal	= vec4(0, 0, 0, 0);

	final_Normal		+= bones[indices.x] * Normals * bone_Weights.x;
	final_Normal		+= bones[indices.y] * Normals * bone_Weights.y;
	final_Normal		+= bones[indices.z] * Normals * bone_Weights.z;
	final_Normal		+= bones[indices.w] * Normals * bone_Weights.w;

	final_Normal.w		= 1;

	frag_Normal			= final_Normal;

	gl_Position			= projection_view * world * final_Position;
}