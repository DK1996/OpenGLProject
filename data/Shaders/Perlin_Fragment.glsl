#version 410

in vec2 frag_Tex_Coord;

out vec4 frag_Color;

uniform sampler2D perlin_Texture;

void main()
{
	vec4 red	= vec4(1, 0, 0, 1);
	vec4 green	= vec4(0, 1, 0, 1);
	vec4 blue	= vec4(0, 0, 1, 0);
	
	frag_Color		= vec4(red);//(perlin_Texture, frag_Tex_Coord).rrrr;
	//frag_Color.a	= 1;
	//frag_Color	= vec4(frag_Tex_Coord, 0, 1);
}