#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_coord;
layout(location=2) in vec4 bone_Indices;
layout(location=3) in vec4 bone_Weights;

out vec2 frag_tex_coord;

uniform mat4 projection_view;
uniform mat4 world;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main()
{
	frag_tex_coord = tex_coord;

	ivec4 indices = ivec4(bone_Indices);
	vec4 final_Position = vec4(0, 0, 0, 0);

	final_Position += bones[indices.x] * position * bone_Weights.x;
	final_Position += bones[indices.y] * position * bone_Weights.y;
	final_Position += bones[indices.z] * position * bone_Weights.z;
	final_Position += bones[indices.w] * position * bone_Weights.w;

	final_Position.w = 1;

	gl_Position = projection_view * world * final_Position;
}