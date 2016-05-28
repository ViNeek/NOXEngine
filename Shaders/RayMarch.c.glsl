#version 430
#extension GL_ARB_compute_variable_group_size : enable

uniform uvec3 u_Dim;
uniform ivec2 u_VPort;
uniform vec3 u_GridSize;
uniform vec3 u_VoxelSize;
uniform vec3 u_GridMin;
uniform mat4 u_LightMVP;
uniform uint u_Seed;

layout (binding = 4) uniform sampler2D u_DepthTexture;
layout (binding = 0) uniform sampler2D u_FluxTexture;

layout(std430, binding=2) readonly buffer VoxelData {
    uint voxel_data[];
};

layout(std430, binding=3) readonly buffer DistanceField {
    float field_data[];
};

layout(std430, binding=5) writeonly buffer Marcher {
    vec4 march_data[];
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

float getVoxelAt( int x, int y, int z ) {
	//return field_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z];
	return field_data[x * u_Dim.x * u_Dim.y + u_Dim.x * y + z];
}

void setVoxelAt( float value, uint x, uint y, uint z ) {
	//march_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z] = value;
}

void main() {
	vec3 l_GlobalTestPosition = vec3(0, 7, 0);
	float l_DistanceBound = length(u_VoxelSize) * (3 + 0.8);
	//float l_DistanceBound = length(vec3(1, 1, 1)) * 1;
	//float l_DistanceBound = 1;
	vec2 l_PixelNDC;

	vec3 l_PositionInGrid = l_GlobalTestPosition - u_GridMin;

	for ( int f = 0; f < 6; f++ ) {
		for ( int i = 0; i < u_VPort.x; i++ ) {
			l_PixelNDC.x = ( float(i) + 0.5) / float(u_VPort.x);
			for ( int j = 0; j < u_VPort.y; j++ ) {
				l_PixelNDC.y = ( float(j) + 0.5) / float(u_VPort.y);
				int l_Counter = 0;
	            ivec3 l_VoxelCoord = ivec3( l_PositionInGrid / u_VoxelSize );
				vec3 l_Dir = cubeMapPixelToDirection(l_PixelNDC, f);
				
				float l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
				//float l_Distance = 1 - getBinaryVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
				float l_OriginalDistance = l_Distance;
				float l_TotalDistance = 0;
				//l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				//ivec3 l_Transform = ivec3(vec3(l_VoxelCoord) + l_Dir * l_DistanceBound);
				//vec3 l_Transform = l_GlobalTestPosition + l_Dir * l_Distance;
				vec3 l_Transform = l_GlobalTestPosition;
				//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y] = clamp(l_Distance, 0, 6);
				//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y] = clamp(l_Transform.x, 0, 11111);
				//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y + 1] = clamp(l_Transform.y, 0, 11111);
				//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y + 2] = clamp(l_Transform.x, 0, 11111);
				//float l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
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
					l_VoxelCoord = ivec3( (l_Transform - u_GridMin) / u_VoxelSize );
					l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
					//l_Distance = 1 - getBinaryVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
					l_OriginalDistance = l_Distance;
					l_Distance = clamp(l_Distance, 0, l_DistanceBound);
					l_TotalDistance += l_Distance;
					//l_Transform = ivec3(vec3(l_Transform) + ceil(l_Dir * l_Distance));
					l_Transform = l_Transform + l_Dir * l_Distance;
					//l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
					//l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				}

				march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].x = l_VoxelCoord.x;
				march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].y = l_VoxelCoord.y;
				march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].z = l_VoxelCoord.z;

                //vec4 shadow_coords = u_LightMVP * vec4(l_VoxelCoord,1);
                vec4 shadow_coords = u_LightMVP * vec4(l_Transform,1);
                vec4 shadow_coords_up = u_LightMVP * vec4(l_Transform + u_VoxelSize.y,1);
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

				//march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = l_TotalDistance;
				//march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = texture( u_DepthTexture, UVCoords ).x;
				if ( rsm_depth < (max(z-0.01, 0.01)) )
                    march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = 4;
                else
                    march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = 2;
                //if ( texture( u_DepthTexture, UVCoords ).x > 0 )
                //    march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = random(UVCoords, int((UVCoords_up.y - UVCoords.y) * 2048.0f ));
                //else
                //    march_data[f*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = texture( u_FluxTexture, UVCoords ).x;
				/*march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].x = l_Distance;
				l_Distance = getVoxelAt(l_Transform.x, l_Transform.y, l_Transform.z);
				l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				l_Transform = ivec3(vec3(l_Transform) + ceil(l_Dir * l_Distance));
				
				march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].y = l_Distance;
				l_Distance = getVoxelAt(l_Transform.x, l_Transform.y, l_Transform.z);
				l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				l_Transform = ivec3(vec3(l_Transform) + ceil(l_Dir * l_Distance));

				march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].z =  l_Distance;
				l_Distance = getVoxelAt(l_Transform.x, l_Transform.y, l_Transform.z);
				l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				l_Transform = ivec3(vec3(l_Transform) + ceil(l_Dir * l_Distance));

				march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = getVoxelAt(l_Transform.x, l_Transform.y, l_Transform.z);;
				*/			
				//march_data[l_Counter] = l_Distance;	
			}
		}
	}
}