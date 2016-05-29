#version 430

layout(location = 0) out vec4 out_color;

uniform uvec3 GridSize;
uniform vec3 GridMin;
uniform vec3 VoxelSize;

in VertexData {
    vec3 normal;
	vec3 worldcoord;
	//flat in uint depth;
	//flat in uvec3 factors;
    //vec2 uv;
} VertexIn;

flat in uint depth;
flat in int dX;
flat in int dY;
flat in int dZ;
flat in int offX;
flat in int offY;
flat in int offZ;

layout (std430, binding=2) buffer VoxelData
{
	uint voxel_data[];
};

layout (binding = 1, offset = 0) uniform atomic_uint VoxelCount;

void setVoxelAt(int i,int j,int w) {
	//voxel_data[dX * ( i - offX ) + dY * ( j - offY ) + dZ * ( w - offZ )] = 1;

    //memoryBarrierAtomicCounter();
    //memoryBarrier();
    if ( atomicCompSwap(voxel_data[i * GridSize.x * GridSize.y + j * GridSize.x + w], 0 , 1) == 0 )
        atomicCounterIncrement(VoxelCount);

    //atomicCompSwap(voxel_data[i * GridSize.x * GridSize.y + j * GridSize.x + w], 0 , 1);
	//voxel_data[i * GridSize.x * GridSize.y + j * GridSize.x + w] = 1;
    //memoryBarrier();
}

void main()
{

	float zf = clamp(gl_FragCoord.z,0,1);
	int x = int(floor(gl_FragCoord.x));
	int y = int(floor(gl_FragCoord.y));
	int z = int(floor(zf*depth));

	vec3 VoxelWorldCoord = ( VertexIn.worldcoord - GridMin ) / VoxelSize;
	ivec3 VoxelGridCoord = ivec3(VoxelWorldCoord);

	//setVoxelAt(x, y, z);
    /*for ( int f = -1; f < 1; f++ ) {
		for ( int i = -1; i < 1; i++ ) {
			for ( int j = -1; j < 1; j++ ) {
	            setVoxelAt(VoxelGridCoord.x + f, VoxelGridCoord.y + i, VoxelGridCoord.z + j);
	        }
        }
    }*/
    setVoxelAt(VoxelGridCoord.x, VoxelGridCoord.y, VoxelGridCoord.z);

	out_color = vec4(zf,zf,zf,0.0f);
}