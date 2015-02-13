#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;

out vec4 frag_Norm;
out vec4 frag_Position;

uniform mat4 projection_view;

void main()
{
	frag_Norm = normal;
	frag_Position = position;
	gl_Position = projection_view * position;
}