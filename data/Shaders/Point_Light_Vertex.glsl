#version 410

layout(location=0) in vec4 position;

// Position in world-space.
uniform vec3 light_Position;

uniform float light_Radius;

uniform mat4 proj_View;

void main()
{
	gl_Position = proj_View * vec4(position.xyz * light_Radius + light_Position, 1);
}