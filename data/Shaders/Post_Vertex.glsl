// Vertex shader!
#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_Coord;

out vec2 frag_Tex_Coord;

void main()
{
	gl_Position = position;
	frag_Tex_Coord = tex_Coord;
}