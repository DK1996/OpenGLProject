#version 410

in vec2 frag_Tex_Coord;

in vec4 frag_Normal;
in vec4 frag_Position;

out vec4 frag_Color;

uniform vec3 light_Dir;
uniform vec3 light_Color;

uniform vec3 ambient_Light;

uniform vec3 eye_Pos;

uniform float spec_Power;

uniform sampler2D perlin_Texture;
uniform sampler2D diffuse;

void main()
{
	vec3 color = texture(diffuse, frag_Tex_Coord).xyz;

	vec3 ambient = color * ambient_Light;

	vec3 N = normalize(frag_Normal.xyz);
	vec3 L = normalize(light_Dir);

	float d = max(0.0, dot(N, -L));

	vec3 final_Diffuse = vec3(d) * light_Color * color;

	vec3 E = normalize(eye_Pos - frag_Position.xyz);
	vec3 R = reflect(L, N);

	float s = max(0, dot(R, E));
	s = pow(s, spec_Power);
	vec3 specular = vec3(s) * light_Color * color;

	frag_Color = vec4(ambient + final_Diffuse + specular, 1);

	//frag_Color = texture(diffuse, frag_Tex_Coord);
	
	//vec4 red	= vec4(1, 0, 0, 1);
	//vec4 green	= vec4(0, 1, 0, 1);
	//vec4 blue	= vec4(0, 0, 1, 0);
	
	//frag_Color		= vec4(1, 1, 1, 1);//(perlin_Texture, frag_Tex_Coord).rrrr;
	//frag_Color.a	= 1;
	//frag_Color	= vec4(frag_Tex_Coord, 0, 1);
}