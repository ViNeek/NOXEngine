#version 430
#extension GL_ARB_compute_variable_group_size : enable

uniform uvec3 dim;

layout(std430, binding=2) readonly buffer VoxelData {
    uint voxel_data[];
};
layout(std430, binding=3) writeonly buffer DistanceField{
    uint field_data[];
};

layout ( local_size_variable ) in;

void setVoxelAt(uint value, unsigned int i,unsigned int j,unsigned int w) {
	field_data[dim.x * i + dim.y * j + dim.z * w] = value;
}

uint getVoxelAt(unsigned int i,unsigned int j,unsigned int w) {
	return voxel_data[dim.x * i + dim.y * j + dim.z * w];
}

void main() {
	
	uint Occupied = getVoxelAt( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z );
	if ( Occupied == 0 ) {
		setVoxelAt(1, gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z );
	} else {
		setVoxelAt(0, gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z );
	}

}