#version 410

layout(location=0) in vec4 position;

uniform mat4 light_Matrix;

void main()
{
	gl_Position = light_Matrix * position;
}