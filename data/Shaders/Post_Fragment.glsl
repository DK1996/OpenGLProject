// Fragment shader!
#version 410

in vec2 frag_Tex_Coord;

out vec4 frag_Color;

uniform sampler2D input_Texture;

vec4 BoxBlur()
{
	vec2 texel_uv = 1.0f / textureSize(input_Texture, 0).xy;

	vec4 color = texture(input_Texture, frag_Tex_Coord);

	color += texture(input_Texture, frag_Tex_Coord + texel_uv);
	color += texture(input_Texture, frag_Tex_Coord + vec2(texel_uv.x, 0));
	color += texture(input_Texture, frag_Tex_Coord + vec2(texel_uv.x, -texel_uv.y));
	color += texture(input_Texture, frag_Tex_Coord + vec2(0, -texel_uv.y));
	color += texture(input_Texture, frag_Tex_Coord + vec2(0, texel_uv.y));
	color += texture(input_Texture, frag_Tex_Coord + vec2(-texel_uv.x, texel_uv.y));
	color += texture(input_Texture, frag_Tex_Coord + vec2(-texel_uv.x, 0));
	color += texture(input_Texture, frag_Tex_Coord + vec2(-texel_uv.x, -texel_uv.y));

	color /= 9;

	return color;
}

vec4 Distort()
{
	vec2 mid = vec2(0.5, 0.5);

	float distance_From_Center = distance(frag_Tex_Coord, mid);
	vec2 normalized_Coord = normalize(frag_Tex_Coord - mid);

	float bias = distance_From_Center + sin(distance_From_Center * 100) * 0.01f;

	vec2 new_Coord = mid + normalized_Coord * bias;

	return texture(input_Texture, new_Coord);
}

void main()
{
	frag_Color = Distort() + BoxBlur();
	//frag_Color = texture(input_Texture, frag_Tex_Coord);
}