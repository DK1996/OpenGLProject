#version 410

layout(location=0) in vec4 position;
layout(location=1) in vec4 normal;
layout(location=2) in vec4 tangent;
layout(location=3) in vec2 tex_Coord;

out vec3 frag_Norm;
out vec3 frag_Position;
out vec3 frag_Tangent;
out vec3 frag_Bitangent;
out vec2 frag_Tex_Coord;

uniform mat4 projection_view;

void main()
{
	frag_Position = position.xyz;
	frag_Norm = normal.xyz;
	frag_Tangent = tangent.xyz;
	frag_Bitangent = cross(normal.xyz, tangent.xyz);
	frag_Tex_Coord = tex_Coord;

	gl_Position = projection_view * position;
}