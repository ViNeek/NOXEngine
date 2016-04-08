#version 400
#extension GL_ARB_geometry_shader4 : enable

layout(triangles) in;

layout(triangle_strip, max_vertices = 7) out;

uniform mat3 NormalMatrix;
//uniform mat4 depthMVP;
uniform mat4 MVP;
uniform mat4 uniform_view_proj[3];
uniform uvec3 uniform_size;

//flat out uvec3 factors;
//flat out uint depth;

#define X_AXIS_LAYER 0
#define Y_AXIS_LAYER 1
#define Z_AXIS_LAYER 2

in VertexData {
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 uv;
} VertexIn[];
 
out VertexData {
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 uv;
} VertexOut;
 
void main()
{

	vec4 pos;
	for(int i = 0; i < gl_VerticesIn; i++)
	{
		pos = MVP * gl_in[i].gl_Position;
		//VertexOut.shadow_coords = depthMVP * gl_in[i].gl_Position;
		VertexOut.normal = VertexIn[i].normal;
		VertexOut.uv = VertexIn[i].uv;
		gl_Position = pos;

		EmitVertex();
	}

	EndPrimitive();

}