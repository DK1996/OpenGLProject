#version 410

out float frag_Depth;

void main()
{
	frag_Depth = gl_FragCoord.z;
}