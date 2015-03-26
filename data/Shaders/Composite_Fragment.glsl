#version 410

in vec2 frag_Tex_Coord;

out vec4 out_Color;

uniform sampler2D albedo_Texture;
//uniform sampler2D position_Texture;
//uniform sampler2D normals_Texture;
uniform sampler2D light_Texture;

void main()
{
	vec4 albedo_Sample = texture(albedo_Texture, frag_Tex_Coord);
	vec4 light_Sample = texture(light_Texture, frag_Tex_Coord);

	out_Color = light_Sample; //* albedo_Sample;
}