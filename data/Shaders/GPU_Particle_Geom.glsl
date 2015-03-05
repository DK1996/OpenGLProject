// Geometry Shader.

#version 410

layout(points) in;
layout(triangle_Strip, max_Vertices =4) out;

in vec3	geo_Position[];
in float geo_Lifespan[];
in float geo_Lifetime[];

out vec4 frag_Color;

uniform mat4 projection_View;
uniform mat4 camera_World;

uniform float start_Size;
uniform float end_Size;

uniform vec4 start_Color;
uniform vec4 end_Color;

void main()
{
	
	float t = geo_Lifetime[0] / geo_Lifespan[0];

	frag_Color = mix(start_Color, end_Color, t);

	float half_Size = mix(start_Size, end_Size, t) * 0.5f;

	vec3 conrners[4];
	conrners[0] = vec3(half_Size,  -half_Size, 0);
	conrners[1] = vec3(half_Size,   half_Size, 0);
	conrners[2] = vec3(-half_Size, -half_Size, 0);
	conrners[3] = vec3(-half_Size,  half_Size, 0);

	vec3 forward	= normalize(camera_World[3].xyz - geo_Position[0]);
	vec3 right		= cross(camera_World[1].xyz, forward);
	vec3 up			= cross(forward, right);

	mat3 billboard_Rot = mat3(right, up, forward);
	
	conrners[0] = billboard_Rot * conrners[0];
	conrners[1] = billboard_Rot * conrners[1];
	conrners[2] = billboard_Rot * conrners[2];
	conrners[3] = billboard_Rot * conrners[3];

	conrners[0] += geo_Position[0];
	conrners[1] += geo_Position[0];
	conrners[2] += geo_Position[0];
	conrners[3] += geo_Position[0];

	gl_Position = projection_View * vec4(conrners[0], 1);
	EmitVertex();
	gl_Position = projection_View * vec4(conrners[1], 1);
	EmitVertex();
	gl_Position = projection_View * vec4(conrners[2], 1);
	EmitVertex();
	gl_Position = projection_View * vec4(conrners[3], 1);
	EmitVertex();
}