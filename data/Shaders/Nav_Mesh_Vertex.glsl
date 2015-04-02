#version 410

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;

out vec4 world_Pos;
out vec4 world_Normal;

uniform mat4 proj_View;

void main()
{
	world_Pos		= vec4(position, 1);
	world_Normal	= vec4(normal, 0);

	gl_Position		= proj_View * world_Pos;
}