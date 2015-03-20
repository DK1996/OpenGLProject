#version 410

in vec4 viewspace_Position;
in vec4 viewspace_Normal;

layout(location=0) out vec3 albedo;
layout(location=1) out vec3 position;
layout(location=2) out vec3 normal;

void main()
{
	position = viewspace_Position.xyz;
	normal = (viewspace_Normal.xyz) * 0.5f + 0.5f;

	albedo = vec3(1);
}