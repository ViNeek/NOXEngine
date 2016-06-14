#version 430
#extension GL_ARB_compute_variable_group_size : enable

uniform uvec3 u_Dim;
uniform ivec2 u_VPort;
uniform uvec3 u_DimCubes;
uniform vec3 u_VoxelSizeCubes;
uniform vec3 u_GridSize;
uniform vec3 u_VoxelSize;
uniform vec3 u_GridMin;
uniform mat4 u_LightMVP;
uniform uint u_Seed;

layout (binding = 7) uniform sampler2D u_NormTexture;
layout (binding = 4) uniform sampler2D u_DepthTexture;
layout (binding = 0) uniform sampler2D u_FluxTexture;

layout (binding = 1, offset = 0) uniform atomic_uint VoxelCount;
layout (binding = 1, offset = 4) uniform atomic_uint BufferIndex;
layout (binding = 1, offset = 8) uniform atomic_uint VoxelCountCubes;

layout(std430, binding=2) readonly buffer VoxelData {
    uint voxel_data[];
};

layout(std430, binding=7) readonly buffer VoxelDataCubes {
    uint voxel_data_cubes[];
};

layout(std430, binding=3) readonly buffer DistanceField {
    float field_data[];
};

layout(std430, binding=5) writeonly buffer Marcher {
    vec4 march_data[];
};

layout(std430, binding=6) writeonly buffer Index {
    int index_data[];
};

layout ( local_size_variable ) in;

int random( vec2 p, int modulo )
{
  // We need irrationals for pseudo randomness.
  // Most (all?) known transcendental numbers will (generally) work.
  const vec2 r = vec2(
    23.1406926327792690,  // e^pi (Gelfond's constant)
     2.6651441426902251); // 2^sqrt(2) (Gelfond–Schneider constant)

  float rndNum = fract( cos( mod( 123456789., 1e-7 + 256. * dot(p,r) ) ) );
  int rndNumi = int( rndNum * 2048.0f );

  return int(mod( rndNumi, modulo ));
  //return 1;
}

vec3 cubeMapPixelToDirection(vec2 texcoord, int face)
{
        vec3 uv = vec3(texcoord, 1);
        uv.xy = 2.0 * texcoord.xy - 1.0;

        vec3 uv2;
        switch(face)
        {
        case 0:
                //positive x
                uv2 = vec3(1.0, -uv.y, -uv.x);
        break;
        case 1:
                //negative x
                uv2 = vec3(-1.0, -uv.y, uv.x);
        break;
        case 2:
                //positive y
                uv2 = vec3(uv.x, 1.0, uv.y);
        break;
        case 3:
                //negative y
                uv2 = vec3(uv.x, -1.0, uv.y);
        break;
        case 4:
                //positive z
                uv2 = vec3(uv.x, -uv.y, 1.0);
        break;
        case 5:
                //negative z
                uv2 = vec3(-uv.x, -uv.y, -1.0);
        break;
        }
        return normalize(uv2);
}

float snoise(in vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float getBinaryVoxelAt( int x, int y, int z ) {
	return voxel_data[x * u_Dim.x * u_Dim.y + u_Dim.x * y + z];
}

float getBinaryVoxelAtCubes( int x, int y, int z ) {
	return voxel_data_cubes[x * u_DimCubes.x * u_DimCubes.y + u_DimCubes.x * y + z];
}

float getVoxelAt( int x, int y, int z ) {
	//return field_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z];
	return field_data[x * u_Dim.x * u_Dim.y + u_Dim.x * y + z];
}

void setVoxelAt( float value, uint x, uint y, uint z ) {
	//march_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z] = value;
}

void setIndexAt( int value, uint x, uint y, uint z ) {
	//index_data[x * u_Dim.x * u_Dim.y + u_Dim.x * y + z] = value;
	index_data[x * u_DimCubes.x * u_DimCubes.y + u_DimCubes.x * y + z] = value;
}

vec4 SampleAt(vec2 coords, int offset) {
    int rndVal = random(coords, offset );
    float rndValF = float(rndVal) / 2048.0;

    vec3 contrib = vec3(0,0,0);

    contrib = texture( u_FluxTexture, vec2(coords.x, coords.y) ).xyz;
    contrib += texture( u_FluxTexture, vec2(coords.x + rndValF, coords.y + rndValF) ).xyz;
    contrib += texture( u_FluxTexture, vec2(coords.x + rndValF, coords.y - rndValF) ).xyz;
    contrib += texture( u_FluxTexture, vec2(coords.x - rndValF, coords.y + rndValF) ).xyz;
    contrib += texture( u_FluxTexture, vec2(coords.x - rndValF, coords.y - rndValF) ).xyz;
    
    return vec4(contrib / 5, 1);
}

void main() {

    bvec3 l_OutOfBounds = greaterThanEqual(gl_GlobalInvocationID, u_Dim);
    bvec3 l_OutOfBoundsCubes = greaterThanEqual(gl_GlobalInvocationID, u_DimCubes);

    //if ( any(l_OutOfBounds) ) {
    if ( any(l_OutOfBoundsCubes) ) {
		return;
	}

    //if ( getBinaryVoxelAt(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y), int(gl_GlobalInvocationID.z)) == 0 ) {
    if ( getBinaryVoxelAtCubes(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y), int(gl_GlobalInvocationID.z)) == 0 ) {
        setIndexAt(-1, gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z);
        return;
    }

    uint l_Index = atomicCounterIncrement(BufferIndex);
    memoryBarrierAtomicCounter();
    memoryBarrier();

    setIndexAt(int(l_Index), gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z);
    uint l_BufferOffset = u_VPort.x * u_VPort.y * 6 * l_Index;
    //uint l_BufferOffset = u_VPort.x * u_VPort.y * 6 * 16 * l_Index;

    uvec3 l_VoxelCoords = uvec3(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y, gl_GlobalInvocationID.z);
	ivec3 l_iVoxelCoords = ivec3(int(gl_GlobalInvocationID.x), int(gl_GlobalInvocationID.y), int(gl_GlobalInvocationID.z));
	//vec3 l_VoxelCenter = vec3(l_iVoxelCoords) * u_VoxelSize + u_VoxelSize * 0.5;
	vec3 l_VoxelCenter = vec3(l_iVoxelCoords) * u_VoxelSizeCubes + u_VoxelSizeCubes * 0.5;

	vec3 l_GlobalTestPosition = vec3(0, 3.5, 0);
	float l_SingleStep = length(u_VoxelSize) * (1 + 0.8);
	float l_DistanceBound = length(u_VoxelSize) * (3 + 0.8);
	//float l_DistanceBound = length(vec3(1, 1, 1)) * 1;
	//float l_DistanceBound = 1;
	vec2 l_PixelNDC;

	//vec3 l_PositionInGrid = l_GlobalTestPosition - u_GridMin;
	vec3 l_PositionInGrid = l_VoxelCenter;
	//vec3 l_PositionInGrid = glm::vec3(63,21,64) * u_VoxelSize + u_VoxelSize * 0.5;
	//vec3 l_PositionInGrid = glm::vec3(64,64,64) * u_VoxelSize + u_VoxelSize * 0.5;

	for ( int f = 0; f < 6; f++ ) {
		for ( int i = 0; i < u_VPort.x; i++ ) {
			l_PixelNDC.x = ( float(i) + 0.5) / float(u_VPort.x);
			for ( int j = 0; j < u_VPort.y; j++ ) {
				l_PixelNDC.y = ( float(j) + 0.5) / float(u_VPort.y);
				int l_Counter = 0;
				vec3 l_Dir = cubeMapPixelToDirection(l_PixelNDC, f);
	            //ivec3 l_VoxelCoord = ivec3( (l_PositionInGrid + l_Dir * l_SingleStep) / u_VoxelSize );
	            ivec3 l_VoxelCoord = ivec3( (l_PositionInGrid + l_Dir * l_SingleStep) / u_VoxelSize );
				
				float l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
				//float l_Distance = 1 - getBinaryVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
				float l_OriginalDistance = l_Distance;
				float l_TotalDistance = 0;
				//l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				//ivec3 l_Transform = ivec3(vec3(l_VoxelCoord) + l_Dir * l_DistanceBound);
				//vec3 l_Transform = l_GlobalTestPosition + l_Dir * l_Distance;
				vec3 l_Transform = l_PositionInGrid;
				//vec3 l_Transform = l_VoxelCenter;
				//vec3 l_Transform = l_VoxelCenter + u_GridMin;
				while ( l_Counter < 100 && l_Distance > 0.0) {
					//l_PositionInGrid = l_VoxelCoord * u_VoxelSize + l_Dir * l_Distance;
					//l_GlobalTestPosition = l_GlobalTestPosition + l_Dir * l_Distance;
					//l_PositionInGrid = l_GlobalTestPosition - u_GridMin;
					//l_VoxelCoord = ivec3( l_PositionInGrid / u_VoxelSize );
					//l_VoxelCoord = ivec3( vec3(l_VoxelCoord) + ceil(l_Dir * l_Distance) );
					//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y] = float(l_VoxelCoord.x) * u_VoxelSize.x;
					//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y + 1] = float(l_VoxelCoord.y) * u_VoxelSize.y;
					//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y + 2] = float(l_VoxelCoord.z) * u_VoxelSize.z;
					l_Counter++;

					l_Distance = clamp(l_Distance, 0, l_DistanceBound);
					//l_Transform = ivec3(vec3(l_Transform) + ceil(l_Dir * l_Distance));
					l_Transform = l_Transform + l_Dir * l_Distance;

					//l_VoxelCoord = ivec3( (l_Transform - u_GridMin) / u_VoxelSize );
					l_VoxelCoord = ivec3( (l_Transform + l_Dir * l_SingleStep) / u_VoxelSize );
					bvec3 l_OutOfBoundsVoxel = greaterThanEqual(l_VoxelCoord, u_Dim);

					if ( any(l_OutOfBoundsVoxel) ) {
						//break;
					}

					l_TotalDistance += l_Distance;
					l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
					//l_Distance = 1 - getBinaryVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
					//l_OriginalDistance = l_Distance;
					
					//l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
					//l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				}

				march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].x = l_VoxelCoord.x;
				march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].y = l_VoxelCoord.y;
				march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].z = l_VoxelCoord.z;
                //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].xyz = l_VoxelCoord.z;

                //vec4 shadow_coords = u_LightMVP * vec4(l_VoxelCoord,1);
                vec4 shadow_coords = u_LightMVP * vec4(l_Transform + u_GridMin,1);
                vec4 shadow_coords_up = u_LightMVP * vec4(l_Transform + u_GridMin + u_VoxelSize.y * 0.5,1);
                vec3 ProjCoords = shadow_coords.xyz / shadow_coords.w;
                vec3 ProjCoords_up = shadow_coords_up.xyz / shadow_coords_up.w;
                vec2 UVCoords;
                vec2 UVCoords_up;
                UVCoords.x = 0.5 * ProjCoords.x + 0.5;
                UVCoords.y = 0.5 * ProjCoords.y + 0.5;
                UVCoords_up.x = 0.5 * ProjCoords_up.x + 0.5;
                UVCoords_up.y = 0.5 * ProjCoords_up.y + 0.5;
                float z = 0.5 * ProjCoords.z + 0.5;
                float z_up = 0.5 * ProjCoords_up.z + 0.5;
                float rsm_depth = texture( u_DepthTexture, UVCoords ).x;

				vec3 texNorm = texture( u_NormTexture, UVCoords ).xyz;

				float LtoT = max(-dot(texNorm, l_Dir), 0);

				//if ( LtoT > 0 ) {

				//march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = l_TotalDistance;
				//march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = texture( u_DepthTexture, UVCoords ).x;
				if ( rsm_depth < (max(z-0.0001, 0.01)) ) {
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = random(UVCoords, int((UVCoords_up.y - UVCoords.y) * 2048.0f) );
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = 1;
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j] = SampleAt(UVCoords, int((UVCoords_up.y - UVCoords.y) * 2048.0f));
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j] = vec4(0,0,0,1);
                } else {
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = random(UVCoords, int((UVCoords_up.y - UVCoords.y) * 2048.0f) );
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = int((UVCoords_up.y - UVCoords.y) * 2048.0f);
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j] = vec4(0,0,0,1);
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j] = SampleAt(UVCoords, int((UVCoords_up.y - UVCoords.y) * 2048.0f)) * sign(LtoT);
                    //march_data[l_BufferOffset + f*u_VPort.x*u_VPort.y + i * u_VPort.y + j] = SampleAt(UVCoords, int((UVCoords_up.y - UVCoords.y) * 2048.0f));
                }
				//}
			}
		}
	}
}