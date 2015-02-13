#version 410

in vec3 frag_Norm;
in vec3 frag_Position;
in vec3 frag_Tangent;
in vec3 frag_Bitangent;
in vec2 frag_Tex_Coord;

out vec4 frag_Color;

uniform vec3 light_Dir;
uniform vec3 light_Color;
uniform vec3 ambient_Light;

uniform vec3 eye_Pos;

uniform float spec_Power;

uniform sampler2d diff_Tex;
uniform sampler2d norm_Tex;
uniform sampler2d spec_Tex;

void main()
{
	mat3 TBN = mat3(normalize(frag_Tangent), normalize(frag_Bitangent), normalize(frag_Norm));
	
	vec3 sampled_Normal = texture(norm_Tex, frag_Tex_Coord).xyz;
	vec3 adjusted_Normal = sampled_Normal * 2 -1;

	vec3 material_Color = texture(diff_Tex, frag_Tex_Coord).xyz
	vec3 ambient = material_Color * ambient_Light;
	
	vec3 N = normalize(TBN * adjusted_Normal);
	vec3 L = normalize(light_Dir);

	float d = max(0.0, dot(N, -L));

	vec3 final_Diffuse = vec3(d) * light_Color * material_Color;
	
	vec3 E = normalize(eye_Pos - frag_Position.xyz);
	vec3 R = reflect(L, N);

	float s = max(0, dot(R, E));

	vec3 material_Spec = texture(spec_Tex, frag_Tex_Coord).xyz;
	s = pow(s, spec_Power);
	vec3 specular = vec3(s) * light_Color * material_Color;

	frag_Color = vec4(ambient + final_Diffuse + specular, 1);
}