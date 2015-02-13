#version 410

in vec4 frag_Norm;
in vec4 frag_Position;
out vec4 frag_Color;

uniform vec3 light_Dir;
uniform vec3 light_Color;

uniform vec3 ambient_Light;

uniform vec3 material_Color;

uniform vec3 eye_Pos;

uniform float spec_Power;

void main()
{
	vec3 ambient = material_Color * ambient_Light;

	vec3 N = normalize(frag_Norm.xyz);
	vec3 L = normalize(light_Dir);

	float d = max(0.0, dot(N, -L));

	vec3 final_Diffuse = vec3(d) * light_Color * material_Color;
	
	vec3 E = normalize(eye_Pos - frag_Position.xyz);
	vec3 R = reflect(L, N);

	float s = max(0, dot(R, E));
	s = pow(s, spec_Power);
	vec3 specular = vec3(s) * light_Color * material_Color;

	frag_Color = vec4(ambient + final_Diffuse + specular, 1);
}