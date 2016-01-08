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
    //vec2 uv;
} VertexIn[];
 
out VertexData {
    vec3 normal;
    //vec4 shadow_coords;
    //vec2 uv;
} VertexOut;
 
void main()
{
/*
	// normal
	vec3 v0 = gl_in[0].gl_Position.xyz;
	vec3 v1 = gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[2].gl_Position.xyz;
	vec3 normal0 = normalize(cross(v1 - v0, v2 - v0));

	vec3 absnormal = abs(normal0);
	int index = X_AXIS_LAYER;
	uvec3 facts = uvec3(dim.x*dim.y, dim.y, 1);
	if (absnormal.y >= absnormal.x && absnormal.y >= absnormal.z) {
		index = Y_AXIS_LAYER;
		facts = uvec3(dim.y, 1, dim.x*dim.y);
	}
	else if (absnormal.z >= absnormal.x && absnormal.z >= absnormal.y) {
		index = Z_AXIS_LAYER;
		facts = uvec3(1, dim.x*dim.y, dim.y);
	}

	// No more layered
	//gl_Layer = index;
	gl_ViewportIndex = index;
	depth = uniform_size[index];
	gl_Position = uniform_view_proj[index] * gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = uniform_view_proj[index] * gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = uniform_view_proj[index] * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
*/

	vec4 pos;
	for(int i = 0; i < gl_VerticesIn; i++)
	{
		pos = MVP * gl_in[i].gl_Position;
		//VertexOut.shadow_coords = depthMVP * gl_in[i].gl_Position;
		VertexOut.normal = VertexIn[i].normal;
		//VertexOut.uv = VertexIn[i].uv;
		gl_Position = pos;

		EmitVertex();
	}

	EndPrimitive();
	/*
	gl_Position = vec4(300,300,300,0);
	VertexOut.normal = VertexIn[0].normal;
	EmitVertex();
	gl_Position = vec4(0,0,0,0);
	VertexOut.normal = VertexIn[0].normal;
	
	EmitVertex();
	gl_Position = vec4(-300,300,-300,0);
	VertexOut.normal = VertexIn[0].normal;
	
	EmitVertex();
	gl_Position = vec4(-300,300,300,0);
	VertexOut.normal = VertexIn[0].normal;
	
	EmitVertex();
	*/
}