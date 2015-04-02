#version 410

in vec4 world_Pos;
in vec4 world_Normal;

out vec4 frag_Color;

void main()
{
	vec3 color		= vec3(1);

	if (fract(world_Pos.x) < 0.05 || fract(world_Pos.y) < 0.05 || fract(world_Pos.z) < 0.05)
	{
		color		= vec3(0);
	}

	vec3 L			= normalize(vec3(1));
	vec3 N			= normalize(world_Normal.xyz);

	float d			= max(0, dot(L, N)) * 0.75;

	frag_Color.rgb	= color * 0.25 + color * d;
	frag_Color.a	= 1;
}