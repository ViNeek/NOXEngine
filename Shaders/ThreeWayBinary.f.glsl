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

layout(location = 0) out uvec4 out_voxel;
flat in uint depth;
//uniform uint uniform_size;

// mark the x-th bit of the 128 bit unsinged integer as 'occupied'
// first find the channel in which it belongs to (each channel is 32 bit)
// once found, use bitwise left-shifting for marking the appropriate bit
// the position is passed as position + 1 for optimization purposes
uvec4 encodeBinaryVoxel(int position)
{
	// check for positioning to R channel
	position = max(position,0);
	uint voxelId1 = (position > 32 || position == 0)? 0u : 1u << uint(position - 1); 

	// check for positioning to G channel
	position = max(position - 32,0); 
	uint voxelId2 = (position > 32 || position == 0)? 0u : 1u << uint(position - 1); 

	// check for positioning to B channel
	position = max(position - 32,0); 
	uint voxelId3 = (position > 32 || position == 0)? 0u : 1u << uint(position - 1); 

	// check for positioning to A channel
	position = max(position - 32,0); 
	uint voxelId4 = (position > 32 || position == 0)? 0u : 1u << uint(position - 1);
	
	// save the occupancy status of the current voxel
	// the logical OR operator has been set externally so that an occupied voxel is not altered
	return uvec4(voxelId1, voxelId2, voxelId3, voxelId4);
}
#define VER1
// this function is currently called for each different layer (X, Y, Z)
void main(void)
{
	// get z
	float z = clamp(gl_FragCoord.z,0,1); 
#ifdef VER1
	// get an unsigned int in the range of [1, depth];
	// this is set this way (instead of [0, depth] in order for the bit shifting operations to work
	// and to avoid unnecessary if's 
	int position = int(floor(z*depth + 0.5));
	//uint position = uint(floor(z*depth));

	// mark the x-th bit of the 128 bit unsinged integer as occupied
	// first find the channel in which it belongs to (each channel is 32 bit)
	// once found, use bitwise left-shifting for marking the appropriate bit
	out_voxel = encodeBinaryVoxel(position);
	//out_voxel = uvec4(0u, 0u, 0u, 0u);
	//out_voxel[position / 32u] = 1u << (position % 32u);
	//out_voxel.a = 0xFFFFFFFF;
#else
	// get an unsigned int in the range of [0, depth - 1];
	uint position = uint(floor(z*depth));

	// mark the x-th bit of the 128 bit unsinged integer as occupied
	// first find the channel in which it belongs to (each channel is 32 bit)
	// once found, use bitwise left-shifting for marking the appropriate bit
	//out_voxel = encodeBinaryVoxel(position);
	out_voxel = uvec4(0u, 0u, 0u, 0u);
	out_voxel[position / 32u] = 1u << (position % 32u);
#endif
	//out_voxel.rgba = vec4(z,z,z,1);
}
