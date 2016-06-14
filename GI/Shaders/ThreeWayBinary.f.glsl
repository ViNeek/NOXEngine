#version 430

layout(location = 0) out vec4 out_color;

uniform uvec3 GridSize;
uniform vec3 GridMin;
uniform vec3 VoxelSize;
uniform uvec3 GridSizeCubes;
uniform vec3 VoxelSizeCubes;
uniform mat4 MVP;

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

layout (std430, binding=7) buffer VoxelDataCubes
{
	uint voxel_data_cubes[];
};

layout (binding = 1, offset = 0) uniform atomic_uint VoxelCount;
layout (binding = 1, offset = 8) uniform atomic_uint VoxelCountCubes;

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

void setVoxelAtCubes(int i,int j,int w) {
	//voxel_data[dX * ( i - offX ) + dY * ( j - offY ) + dZ * ( w - offZ )] = 1;

    //memoryBarrierAtomicCounter();
    //memoryBarrier();
    if ( atomicCompSwap(voxel_data_cubes[i * GridSizeCubes.x * GridSizeCubes.y + j * GridSizeCubes.x + w], 0 , 1) == 0 )
        atomicCounterIncrement(VoxelCountCubes);

    //atomicCompSwap(voxel_data[i * GridSize.x * GridSize.y + j * GridSize.x + w], 0 , 1);
	//voxel_data[i * GridSize.x * GridSize.y + j * GridSize.x + w] = 1;
    //memoryBarrier();
}

bool in_frustum(mat4 M, vec3 p) {
return true;
        vec4 Pclip = M * vec4(p, 1);
        return abs(Pclip.x) < Pclip.w+0.5 && 
               abs(Pclip.y) < Pclip.w+0.5 && 
               -1 < Pclip.z && 
               Pclip.z < Pclip.w+0.5;
    }

void main()
{

	float zf = clamp(gl_FragCoord.z,0,1);
	int x = int(floor(gl_FragCoord.x));
	int y = int(floor(gl_FragCoord.y));
	int z = int(floor(zf*depth));

	vec3 VoxelWorldCoord = ( VertexIn.worldcoord - GridMin ) / VoxelSize;
	ivec3 VoxelGridCoord = ivec3(floor(VoxelWorldCoord));
    vec3 VoxelWorldCoordCubes = ( VertexIn.worldcoord - GridMin ) / VoxelSizeCubes;
	ivec3 VoxelGridCoordCubes = ivec3(floor(VoxelWorldCoordCubes));

    //VoxelGridCoord.y+=15;
	//setVoxelAt(x, y, z);
    /*for ( int f = -1; f < 1; f++ ) {
		for ( int i = -1; i < 1; i++ ) {
			for ( int j = -1; j < 1; j++ ) {
	            setVoxelAt(VoxelGridCoord.x + f, VoxelGridCoord.y + i, VoxelGridCoord.z + j);
	        }
        }
    }*/
    setVoxelAt(VoxelGridCoord.x, VoxelGridCoord.y, VoxelGridCoord.z);

    if (in_frustum( MVP, VoxelGridCoordCubes * VoxelSizeCubes  + GridMin ) ) {
        setVoxelAtCubes(VoxelGridCoordCubes.x, VoxelGridCoordCubes.y, VoxelGridCoordCubes.z);
        setVoxelAtCubes(VoxelGridCoordCubes.x, VoxelGridCoordCubes.y, VoxelGridCoordCubes.z+1);
        setVoxelAtCubes(VoxelGridCoordCubes.x, VoxelGridCoordCubes.y, VoxelGridCoordCubes.z-1);
        setVoxelAtCubes(VoxelGridCoordCubes.x, VoxelGridCoordCubes.y+1, VoxelGridCoordCubes.z);
        setVoxelAtCubes(VoxelGridCoordCubes.x, VoxelGridCoordCubes.y-1, VoxelGridCoordCubes.z);
        setVoxelAtCubes(VoxelGridCoordCubes.x+1, VoxelGridCoordCubes.y, VoxelGridCoordCubes.z);
        setVoxelAtCubes(VoxelGridCoordCubes.x-1, VoxelGridCoordCubes.y, VoxelGridCoordCubes.z);
    }
    //setVoxelAt(VoxelGridCoord.x+1, VoxelGridCoord.y, VoxelGridCoord.z);
    //setVoxelAt(VoxelGridCoord.x-1, VoxelGridCoord.y, VoxelGridCoord.z);
    //setVoxelAt(VoxelGridCoord.x, VoxelGridCoord.y+1, VoxelGridCoord.z);
    //setVoxelAt(VoxelGridCoord.x, VoxelGridCoord.y-1, VoxelGridCoord.z);
    //setVoxelAt(VoxelGridCoord.x+1, VoxelGridCoord.y+1, VoxelGridCoord.z);
    //setVoxelAt(VoxelGridCoord.x-1, VoxelGridCoord.y-1, VoxelGridCoord.z);
    //setVoxelAt(VoxelGridCoord.x, VoxelGridCoord.y, VoxelGridCoord.z-1);
    //setVoxelAt(VoxelGridCoord.x, VoxelGridCoord.y, VoxelGridCoord.z+1);
	//out_color = vec4(zf,zf,zf,0.0f);
}