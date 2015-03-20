#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;

out vec4 viewspace_Position;
out vec4 viewspace_Normal;

uniform mat4 view;
uniform mat4 proj_View;

void main()
{
	viewspace_Position = view * vec4(position.xyz, 1);
	viewspace_Normal = view * vec4(normal.xyz, 0);

	gl_Position = proj_View * position;
}