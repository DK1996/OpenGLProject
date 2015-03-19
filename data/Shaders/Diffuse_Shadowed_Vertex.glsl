#version 410

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

out vec4 frag_Normal;
out vec4 frag_Shadow_Coord;

uniform mat4 view_Projection;
uniform mat4 light_Matrix;

void main()
{
	frag_Normal = normal;
	gl_Position = view_Projection * position;

	frag_Shadow_Coord = light_Matrix * position;
}