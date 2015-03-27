#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_Coord;

out vec2 frag_Tex_Coord;

uniform mat4 view_Proj;
uniform float time;

uniform sampler2D perlin_Texture;

void main()
{
	vec4 pos		= position;
	pos.y			+= texture(perlin_Texture, tex_Coord).r * 15;

	frag_Tex_Coord	= tex_Coord;
	gl_Position		= view_Proj * pos;
}