#version 410

layout(location=0) in vec3 position;
layout(location=1) in vec3 velocity;
layout(location=2) in float lifespan;
layout(location=3) in float lifetime;

out vec3 updated_Position;
out vec3 updated_Velocity;
out float updated_Lifespan;
out float updated_Lifetime;

uniform float delta_Time;

uniform vec3 emitter_Position;

uniform float min_Velocity;
uniform float max_Velocity;
uniform float min_Lifespan;
uniform float max_Lifespan;

uniform float time;

const float INVERSE_MAX_UNIT = 1.0f / 4294967295.0f;

float rand(uint seed, float range)
{
	uint i = (seed ^ 12345391u) * 2654435769u;
	i ^= (i << 6u) ^ (i >> 26u);
	i *= 2654435769u;
	i += (i << 5u) ^ (i >> 12u);
	
	return float (range * i) * INVERSE_MAX_UNIT;
}

void main()
{
	// Move particles based on velocity.
	updated_Position = position + velocity * delta_Time;
	updated_Velocity = velocity;
	updated_Lifespan = lifespan;
	
	// Update the lifetime based on delat time.
	updated_Lifetime = lifetime + delta_Time;

	// Spawn new particles if we need to.	
	if (updated_Lifetime > lifespan)
	{
		updated_Position = emitter_Position;

		// Generating random seed.
		uint seed = uint(gl_VertexID + (time * 1000));

		// Set its velocity.
		float vel_Range = max_Velocity - min_Velocity;
		float vel_Len = rand(++seed, vel_Range) + min_Velocity;

		updated_Velocity.x = rand(++seed, 2) - 1;
		updated_Velocity.y = rand(++seed, 2) - 1;
		updated_Velocity.z = rand(++seed, 2) - 1;
		updated_Velocity = normalize(updated_Velocity) * vel_Len;
				
		// Set its lifespan.
		updated_Lifetime = 0;

		float lifespan_Range = max_Lifespan - min_Lifespan;
		updated_Lifespan = rand(++seed, lifespan_Range) + min_Lifespan;
	}
}