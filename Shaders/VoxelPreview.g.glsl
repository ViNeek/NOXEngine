#version 400
#extension GL_ARB_geometry_shader4 : enable

layout(points) in;

layout(points, max_vertices = 3) out;

//uniform mat3 NormalMatrix;
uniform mat4 MVP;
//uniform mat4 depthMVP;

//in VertexData {
    //vec3 normal;
    //vec2 uv;
//} VertexIn[];
 
//out VertexData {
    //vec3 normal;
    //vec4 shadow_coords;
    //vec2 uv;
//} VertexOut;
 
void main()
{
	vec4 pos;
	for(int i = 0; i < gl_VerticesIn; i++)
	{
		pos = MVP * gl_in[i].gl_Position;
		//VertexOut.shadow_coords = depthMVP * gl_in[i].gl_Position;
		//VertexOut.normal = VertexIn[i].normal;
		//VertexOut.uv = VertexIn[i].uv;
		gl_Position = pos;

		EmitVertex();
	}

	EndPrimitive();
}