#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_Coord;
layout(location=2) in vec4 Normals;

out vec2 frag_Tex_Coord;
out vec4 frag_Normal;
out vec4 frag_Position;

uniform mat4 view_Proj;
uniform float time;

uniform sampler2D perlin_Texture;

void main()
{
	frag_Position	= position;
	
	vec4 pos		= position;
	pos.y			+= texture(perlin_Texture, tex_Coord).r * 10;

	frag_Normal		= Normals;

	frag_Tex_Coord	= tex_Coord;
	gl_Position		= view_Proj * pos;
}