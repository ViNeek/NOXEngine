#version 430
#extension GL_ARB_compute_variable_group_size : enable

uniform uvec3 u_Dim;
uniform int u_Truncation;

layout(std430, binding=2) readonly buffer VoxelData {
    uint voxel_data[];
};
layout(std430, binding=3) writeonly buffer DistanceField {
    uint field_data[];
};

layout ( local_size_variable ) in;

void setVoxelAt( uint value, uint x, uint y, uint z ) {
	field_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z] = value;
}

uint getVoxelAt( uint x, uint y, uint z ) {
	return voxel_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z];
}

void calculateDistanceFieldAt(uint x, uint y, uint z) {
	//int l_Step = 2 * u_Truncation;
	int l_Step = u_Truncation;
	for ( int i = -u_Truncation; i <= u_Truncation; i+=l_Step ) {
		if ( ( x + i ) > u_Dim.x ) 
			continue;
		for ( int j = -u_Truncation; j <= u_Truncation; j+=l_Step ) {
			if ( ( y + j ) > u_Dim.y ) 
				continue;
			for ( int k = -u_Truncation; k <= u_Truncation; k+=l_Step ) {
				if ( ( z + k ) > u_Dim.z ) 
					continue;

				uvec3 l_Neighbour = uvec3(x + i, y + j, z + k);

				uint l_Occupied = getVoxelAt( l_Neighbour.x,
					                          l_Neighbour.y,
											  l_Neighbour.z );

				if ( l_Occupied == 1 ) {
					setVoxelAt(1, x,
					              y,
								  z);
					
					setVoxelAt(0, l_Neighbour.x,
					              l_Neighbour.y,
								  l_Neighbour.z );

					return;
				}
			}
		}
	}
}

void main() {
	
	bvec3 l_OutOfBounds = greaterThanEqual(gl_GlobalInvocationID, u_Dim);

	if ( any(l_OutOfBounds) ) {
		return;
	}

	//uint l_Occupied = getVoxelAt( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z );
	
	calculateDistanceFieldAt( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z );

}