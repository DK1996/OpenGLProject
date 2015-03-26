#version 410

out vec3 light_Output;

uniform vec3 light_View_Pos;
uniform vec3 light_Diffuse;

uniform float light_Radius;

uniform sampler2D position_Texture;
uniform sampler2D normal_Texture;

void main()
{
	// Texture coord.
	vec2 tex_Coord = gl_FragCoord.xy / textureSize(position_Texture, 0).xy;
	
	// Sample from textures.
	vec3 position_Sample = texture(position_Texture, tex_Coord).xyz;
	vec3 normal_Sample = texture(normal_Texture, tex_Coord).xyz;
	normal_Sample *= 2;
	normal_Sample -= 1;

	// Compute light direction.
	vec3 to_Light = light_View_Pos - position_Sample;
	vec3 L = normalize(to_Light);

	// Compute lambertian term.
	float d = max(0, dot(normal_Sample, L));

	// Compute falloff.
	float falloff = 1 - min(1, length(to_Light) / light_Radius);

	// Output lambert * falloff * color.
	light_Output = d * falloff * light_Diffuse;
}