// Vetex Shader.

#version 410

layout(location=0) in vec3 position;
layout(location=1) in vec3 velocity;
layout(location=2) in float lifespan;
layout(location=3) in float lifetime;

out vec3 geo_Position;
out float geo_Lifespan;
out float geo_Lifetime;

void main()
{
	geo_Position = position;
	geo_Lifespan = lifespan;
	geo_Lifetime = lifetime;
}