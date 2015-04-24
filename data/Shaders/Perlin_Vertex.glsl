#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec2 tex_Coord;

out vec2 frag_Tex_Coord;
out vec4 frag_Normal;
out vec4 frag_Position;

uniform mat4 view_Proj;
uniform float time;

uniform sampler2D perlin_Texture;

void main()
{
	int offset = 50;

	frag_Position	= position;
	
	vec4 pos		= position;
	pos.y			+= texture(perlin_Texture, tex_Coord).r * 10;

	frag_Tex_Coord	= tex_Coord;
	gl_Position		= view_Proj * pos;

	float texture_Size = textureSize(perlin_Texture, 0);
	float texture_Offset = 1 / texture_Size;
	float sample_Size = 1.0f;

	vec4 up = position;
	up.y += texture(perlin_Texture, tex_Coord + vec2(0, texture_Offset)).r * offset;
	up.z += sample_Size;

	vec4 down = position;
	down.y += texture(perlin_Texture, tex_Coord + vec2(0, -texture_Offset)).r * offset;
	down.z -= sample_Size;

	vec4 right = position;
	right.x += sample_Size;
	right.y += texture(perlin_Texture, tex_Coord + vec2(texture_Offset, 0)).r * offset;

	vec4 left = position;
	left.x -= sample_Size;
	left.y += texture(perlin_Texture, tex_Coord + vec2(-texture_Offset, 0)).r * offset;

	vec4 up_Right = position;
	up_Right.x += sample_Size;
	up_Right.y += texture(perlin_Texture, tex_Coord + vec2(texture_Offset, texture_Offset)).r * offset;
	up_Right.z += sample_Size;

	vec4 up_Left = position;
	up_Left.x -= sample_Size;
	up_Left.y += texture(perlin_Texture, tex_Coord + vec2(-texture_Offset, texture_Offset)).r * offset;
	up_Left.z += sample_Size;

	vec4 down_Right = position;
	down_Right.x += sample_Size;
	down_Right.y += texture(perlin_Texture, tex_Coord + vec2(texture_Offset, -texture_Offset)).r * offset;
	down_Right.z -= sample_Size;

	vec4 down_Left = position;
	down_Left.x -= sample_Size;
	down_Left.y += texture(perlin_Texture, tex_Coord + vec2(-texture_Offset, -texture_Offset)).r * offset;
	down_Left.z -= sample_Size;

	vec4 final_Normal = vec4(0);

	final_Normal.xyz += cross(up.xyz - pos.xyz, up_Right.xyz - pos.xyz);
	final_Normal.xyz += cross(up_Right.xyz - pos.xyz, right.xyz - pos.xyz);
	final_Normal.xyz += cross(right.xyz - pos.xyz, down_Right.xyz - pos.xyz);
	final_Normal.xyz += cross(down_Right.xyz - pos.xyz, down.xyz - pos.xyz);

	final_Normal.xyz += cross(down.xyz - pos.xyz, down_Left.xyz - pos.xyz);
	final_Normal.xyz += cross(down_Left.xyz - pos.xyz, left.xyz - pos.xyz);
	final_Normal.xyz += cross(left.xyz - pos.xyz, up_Left.xyz - pos.xyz);
	final_Normal.xyz += cross(up_Left.xyz - pos.xyz, up.xyz - pos.xyz);

	frag_Normal = normalize(final_Normal);
}