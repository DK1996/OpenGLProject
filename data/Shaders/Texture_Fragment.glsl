#version 410

in vec2 frag_tex_coord;
in vec4 reflected_screen_pos;

out vec4 frag_color;

uniform sampler2D diffuse;

void main()
{
	vec4 uv_pos = reflected_screen_pos / reflected_screen_pos.w;
	uv_pos = (uv_pos + 1) * 0.5f;
	frag_color = texture(diffuse, uv_pos.xy);
}