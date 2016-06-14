#version 400
#extension GL_ARB_geometry_shader4 : enable

layout(triangles) in;

layout(triangle_strip, max_vertices = 7) out;

uniform mat3 NormalMatrix;
//uniform mat4 depthMVP;
uniform mat4 MVP;
uniform mat4 ModelViewMatrix;
uniform mat4 uniform_view_proj[3];
uniform uvec3 uniform_size;
uniform mat4 lightMVP;

in VertexData {
    vec3 world_pos;
    vec4 ecPos;
    vec4 shadow_coords;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 uv;
} VertexIn[];
 
out VertexData {
    vec3 world_pos;
    vec4 ecPos;
    vec4 shadow_coords;
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
        VertexOut.shadow_coords = lightMVP * VertexIn[i].shadow_coords;
        VertexOut.ecPos = ModelViewMatrix * VertexIn[i].ecPos;
        //VertexOut.ecPos = VertexIn[i].ecPos;
		//VertexOut.shadow_coords = depthMVP * gl_in[i].gl_Position;
		VertexOut.normal = VertexIn[i].normal;
		//VertexOut.NdotL = VertexIn[i].NdotL;
		VertexOut.world_pos = VertexIn[i].world_pos;
        
		VertexOut.uv = VertexIn[i].uv;
		gl_Position = pos;

		EmitVertex();
	}

	EndPrimitive();

}