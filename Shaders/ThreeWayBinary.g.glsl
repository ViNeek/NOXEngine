#version 400
#extension GL_ARB_viewport_array : enable
#extension GL_ARB_geometry_shader4 : enable

layout(triangles) in;

layout(triangle_strip, max_vertices = 7) out;

uniform mat3 NormalMatrix;
//uniform mat4 depthMVP;
uniform mat4 MVP;
uniform mat4 ViewProjMatrix[3];
uniform uvec3 GridSize;

//flat out uvec3 factors;
flat out uint depth;
flat out int dX;
flat out int dY;
flat out int dZ;
flat out int offX;
flat out int offY;
flat out int offZ;

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

// normal
	vec3 v0 = gl_in[0].gl_Position.xyz;
	vec3 v1 = gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[2].gl_Position.xyz;
	vec3 normal0 = normalize(cross(v1 - v0, v2 - v0));

	vec3 absnormal = abs(normal0);

	//uvec3 facts = uvec3(GridSize.x*GridSize.y, GridSize.y, 1);
	//uvec3 facts = uvec3(GridSize.y, 1, GridSize.x*GridSize.y);
	//uvec3 facts = uvec3(1, GridSize.x*GridSize.y, GridSize.y);
	//int index = X_AXIS_LAYER;
	//dX = 1;
	//dY = GridSize.x;
	//dZ = GridSize.x * GridSize.z;
	//offX = 0;
	//offY = 0;
	//offZ = 0;
	//index = X_AXIS_LAYER;
	/*index = Z_AXIS_LAYER;
		dX = 1;
		dY = GridSize.y;
		dZ = GridSize.y * GridSize.x;*/
	/*
	index = Y_AXIS_LAYER;
		dX = 1;
		dY = GridSize.y*GridSize.z;
		dZ = GridSize.y;*/
	int index = X_AXIS_LAYER;
	dX = 1;
	dY = int(GridSize.x);
	dZ = int(GridSize.x) * int(GridSize.y);
	offX = 0;
	offY = 0;
	offZ = 0;
	if (absnormal.y >= absnormal.x && absnormal.y >= absnormal.z) {
		index = Y_AXIS_LAYER;
		dX = -1;
		dY = -int(GridSize.x) * int(GridSize.z);
		dZ = -int(GridSize.x);
		offX = int(GridSize.x-1);
		offY = int(GridSize.y-1);
		offZ = int(GridSize.z-1);
		//facts = uvec3(GridSize.y, 1, GridSize.x*GridSize.y);
	}
	else if (absnormal.z >= absnormal.x && absnormal.z >= absnormal.y) {
		index = Z_AXIS_LAYER;
		dX = int(GridSize.x) * int(GridSize.z);
		dY = int(GridSize.x);
		dZ = -1;
		offX = 0;
		offY = 0;
		offZ = int(GridSize.z-1);
		//facts = uvec3(1, GridSize.x*GridSize.y, GridSize.y);
	}

	// No more layered
	//gl_Layer = index;
	gl_ViewportIndex = index;
	depth = GridSize[index];
	//factors = facts;
	gl_Position = ViewProjMatrix[index] * gl_in[0].gl_Position;
	EmitVertex();
	gl_ViewportIndex = index;
	depth = GridSize[index];
	//factors = facts;
	//dX = GridSize.x;
	//dY = GridSize.y;
	//dZ = GridSize.z;
	gl_Position = ViewProjMatrix[index] * gl_in[1].gl_Position;
	EmitVertex();
	gl_ViewportIndex = index;
	depth = GridSize[index];
	//factors = facts;
	//dX = GridSize.x;
	//dY = GridSize.y;
	//dZ = GridSize.z;
	gl_Position = ViewProjMatrix[index] * gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();

	/*
	vec4 pos;
	for(int i = 0; i < gl_VerticesIn; i++)
	{
		pos = gl_in[i].gl_Position;
		//VertexOut.shadow_coords = depthMVP * gl_in[i].gl_Position;
		VertexOut.normal = VertexIn[i].normal;
		//VertexOut.uv = VertexIn[i].uv;
		gl_Position = pos;

		EmitVertex();
	}
	*/

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