//----------------------------------------------------//
//                                                    //
// This is a free rendering engine. The library and   //
// the source code are free. If you use this code as  //
// is or any part of it in any kind of project or     //
// product, please acknowledge the source and its	  //
// author.											  //
//                                                    //
// For manuals, help and instructions, please visit:  //
// http://graphics.cs.aueb.gr/graphics/               //
//                                                    //
//----------------------------------------------------//
#version 330 core
#extension GL_ARB_viewport_array : enable

layout(triangles) in;
layout (point, max_vertices=9) out;

uniform mat4 uniform_view_proj[3];
uniform uvec3 uniform_size;

flat out uvec3 factors;
flat out uint depth;

#define X_AXIS_LAYER 0
#define Y_AXIS_LAYER 1
#define Z_AXIS_LAYER 2

//#define THREEAXISVOXELIZATION
#define DOMINANTAXISVOXELIZATION

void main()
{
#ifdef THREEAXISVOXELIZATION
	// Three axis voxelization
	for (int i = 0; i < 3; ++i)
	{
		gl_Layer = i;
		gl_ViewportIndex = i;	
		depth = uniform_size[i];

		for(int j = 0; j < gl_in.length(); j++)
		{
			// copy attributes
			gl_Position = uniform_view_proj[i] * gl_in[j].gl_Position;
	
			// done with the vertex
			EmitVertex();
		}
		// done with the primitive
		EndPrimitive();
	}

	// serialized
	
	/*
	// this is slightly faster
	gl_Layer = 0;
	gl_ViewportIndex = 0;
	depth = uniform_size[0];
	gl_Position = uniform_view_proj[0] * gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = uniform_view_proj[0] * gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = uniform_view_proj[0] * gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
	gl_Layer = 1;
	gl_ViewportIndex = 1;
	depth = uniform_size[1];
	gl_Position = uniform_view_proj[1] * gl_in[0].gl_Position;
	EmitVertex();						 
	gl_Position = uniform_view_proj[1] * gl_in[1].gl_Position;
	EmitVertex();						 
	gl_Position = uniform_view_proj[1] * gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
	gl_Layer = 2;
	gl_ViewportIndex = 2;
	depth = uniform_size[2];
	gl_Position = uniform_view_proj[2] * gl_in[0].gl_Position;
	EmitVertex();					
	gl_Position = uniform_view_proj[2] * gl_in[1].gl_Position;
	EmitVertex();					
	gl_Position = uniform_view_proj[2] * gl_in[2].gl_Position;
	EmitVertex();
	EndPrimitive();
	*/
#endif // THREEAXISVOXELIZATION

#ifdef DOMINANTAXISVOXELIZATION
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

#endif // DOMINANTAXISVOXELIZATION

}
