#version 410

in vec2 frag_tex_coord;

in vec4 frag_Normal;
in vec4 frag_Position;

out vec4 frag_color;

uniform vec3 light_Dir;
uniform vec3 light_Color;

uniform vec3 ambient_Light;

uniform vec3 eye_Pos;

uniform float spec_Power;

uniform sampler2D diffuse;

void main()
{
	vec3 color = texture(diffuse, frag_tex_coord).xyz;

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
	
	frag_color = vec4(ambient + final_Diffuse + specular, 1);

	//frag_color = texture(diffuse, frag_tex_coord);

	//frag_color= frag_Normal;
}