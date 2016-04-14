#version 430
#extension GL_ARB_compute_variable_group_size : enable

uniform uvec3 u_Dim;
uniform vec3 u_VoxelSize;

layout(std430, binding=3) readonly buffer DistanceFieldFront {
    float front_field_data[];
};
layout(std430, binding=4) writeonly buffer DistanceFieldBack {
    float back_field_data[];
};

layout ( local_size_variable ) in;

void setVoxelAt( float value, uint x, uint y, uint z ) {
	back_field_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z] = value;
}

float getVoxelAt( uint x, uint y, uint z ) {
	return front_field_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z];
}

void calculateDistanceFieldAt(uint x, uint y, uint z) {
	uvec3 l_VoxelCoords = uvec3(x, y, z);
	vec3 l_VoxelCenter = vec3(l_VoxelCoords) * u_VoxelSize + u_VoxelSize * 0.5;
	float l_MinDistance = 20000;
	uint l_X;
	uint l_Y;
	uint l_Z;

	for ( int i = -1; i <= 1; i++ ) {
		l_X = x + i;
		for ( int j = -1; j <= 1; j++ ) {
			l_Y = y + j;
			for ( int k = -1; k <= 1; k++ ) {
				l_Z = z + k;

				uvec3 l_Neighbour = uvec3(
						clamp(l_X, 0, u_Dim.x - 1),
						clamp(l_Y ,0, u_Dim.y - 1), 
						clamp(l_Z, 0, u_Dim.z - 1));

				float l_NeighbourDist = getVoxelAt( l_Neighbour.x,
					                              l_Neighbour.y,
											      l_Neighbour.z );
				vec3 l_NeighbourCenter = vec3(l_Neighbour) * u_VoxelSize + u_VoxelSize * 0.5;	

				float l_CurrentDist = length( l_NeighbourCenter - l_VoxelCenter ) + l_NeighbourDist;
				//float l_CurrentDist = length( vec3( i, j, k ) ) + l_NeighbourDist;

				if ( l_CurrentDist < l_MinDistance ) {
					l_MinDistance = l_CurrentDist;
				}
			}
		}
	}

	setVoxelAt(l_MinDistance, l_VoxelCoords.x,
							  l_VoxelCoords.y,
							  l_VoxelCoords.z );
}

void main() {
	
	bvec3 l_OutOfBounds = greaterThanEqual(gl_GlobalInvocationID, u_Dim);

	if ( any(l_OutOfBounds) ) {
		return;
	}

	//uint l_Occupied = getVoxelAt( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z );
	
	calculateDistanceFieldAt( gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z );

}