#version 410

in vec4 frag_Normal;
in vec4 frag_Shadow_Coord;

out vec4 frag_Color;

uniform vec3 light_Dir;
uniform simpler2D shadow_Map;

void main()
{
	float d = max(0, dot(-light_Dir, normalize(frag_Normal).xyz));

	float dis_From_Light = texture(shadow_Map, frag_Shadow_Coord.xy).r;

	if (texture(shadow_Map, frag_Shadow_Coord.xy).r - 0.01f) { d = 0; }

	frag_Color = vec4(d, d, d, 1);
}