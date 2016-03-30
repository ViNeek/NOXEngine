#version 430

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_color2;

in VertexData {
    vec3 normal;
	//flat in uint depth;
	//flat in uvec3 factors;
    //vec2 uv;
} VertexIn;

void main()
{
	
	out_color = vec4(VertexIn.normal,0.0f);
	out_color2 = vec4(VertexIn.normal / 2,0.0f);

}