#version 430

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_color2;

float atten = 0.8;

in VertexData {
    vec3 normal;
	//flat in uint depth;
	//flat in uvec3 factors;
    //vec2 uv;
} VertexIn;

void main()
{
	
	out_color2 = vec4(vec3(1,0,0)*gl_FragCoord.z,0.0f);
	out_color = vec4(VertexIn.normal, 0.0f);

}