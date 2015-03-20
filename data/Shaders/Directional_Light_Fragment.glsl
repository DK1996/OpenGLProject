#version 410

in vec2 frag_Tex_Coord;

out vec3 out_Color;

uniform vec3 light_Dir;
uniform vec3 light_Color;

uniform sampler2D position_Texture;
uniform sampler2D normal_Texture;

void main()
{
	vec3 normal_Sampler = texture(normal_Texture, frag_Tex_Coord).xyz;
	normal_Sampler *= 2;
	normal_Sampler -= 1;

	vec3 position_Sampler = texture(position_Texture, frag_Tex_Coord).xyz;

	//vec3 N = normalize(normal_Sampler);
	float d = max(0, dot(-light_Dir, normal_Sampler));

	out_Color = light_Color * d;
}