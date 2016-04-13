#version 430
#extension GL_ARB_compute_variable_group_size : enable

uniform uvec3 u_Dim;
uniform ivec2 u_VPort;
uniform vec3 u_GridSize;
uniform vec3 u_VoxelSize;
uniform vec3 u_GridMin;

layout(std430, binding=3) readonly buffer DistanceField {
    float field_data[];
};

layout(std430, binding=5) writeonly buffer Marcher {
    vec4 march_data[];
};

layout ( local_size_variable ) in;

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


float getVoxelAt( int x, int y, int z ) {
	return field_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z];
}

void setVoxelAt( float value, uint x, uint y, uint z ) {
	//march_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z] = value;
}

void main() {
	vec3 l_GlobalTestPosition = vec3(0, 10, 0);
	//float l_DistanceBound = length(u_VoxelSize) * 4;
	float l_DistanceBound = length(vec3(1, 1, 1)) * 1;
	vec2 l_PixelNDC;

	vec3 l_PositionInGrid = l_GlobalTestPosition - u_GridMin;
	ivec3 l_VoxelCoord = ivec3( l_PositionInGrid / u_VoxelSize );

	for ( int f = 0; f < 6; f++ ) {
		for ( int i = 0; i < u_VPort.x; i++ ) {
			l_PixelNDC.x = ( i + 0.5) / u_VPort.x;
			for ( int j = 0; j < u_VPort.y; j++ ) {
				l_PixelNDC.y = ( j + 0.5) / u_VPort.y;
				int l_Counter = 0;
				vec3 l_Dir;
				l_Dir = cubeMapPixelToDirection(l_PixelNDC, f);
				
				float l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
				float l_TotalDistance = 0;
				l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				ivec3 l_Transform = ivec3(vec3(l_VoxelCoord) + ceil(l_Dir * l_Distance));
				//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y] = clamp(l_Distance, 0, 6);
				//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y] = clamp(l_Transform.x, 0, 11111);
				//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y + 1] = clamp(l_Transform.y, 0, 11111);
				//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y + 2] = clamp(l_Transform.x, 0, 11111);
				//float l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
				/*while ( l_Distance > 0 && l_Counter < 20 ) {
					//l_PositionInGrid = l_VoxelCoord * u_VoxelSize + l_Dir * l_Distance;
					//l_GlobalTestPosition = l_GlobalTestPosition + l_Dir * l_Distance;
					//l_PositionInGrid = l_GlobalTestPosition - u_GridMin;
					//l_VoxelCoord = ivec3( l_PositionInGrid / u_VoxelSize );
					l_VoxelCoord = ivec3( vec3(l_VoxelCoord) + ceil(l_Dir * l_Distance) );
					//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y] = float(l_VoxelCoord.x) * u_VoxelSize.x;
					//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y + 1] = float(l_VoxelCoord.y) * u_VoxelSize.y;
					//march_data[f*6*u_VPort.x*u_VPort.y + i + j * u_VPort.y + 2] = float(l_VoxelCoord.z) * u_VoxelSize.z;
					l_Counter++;
					l_TotalDistance += l_Distance;
					l_Distance = getVoxelAt(l_VoxelCoord.x, l_VoxelCoord.y, l_VoxelCoord.z);
					l_Distance = clamp(l_Distance, 0, l_DistanceBound);
				}*/

				//march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].x = float(l_VoxelCoord.x) * u_VoxelSize.x + u_GridMin.x;
				//march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].y = float(l_VoxelCoord.y) * u_VoxelSize.y + u_GridMin.y;
				//march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].z = float(l_VoxelCoord.z) * u_VoxelSize.z + u_GridMin.z;
				//march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = l_TotalDistance;
				march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].x = l_Transform.x;
				march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].y = l_Transform.y;
				march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].z = l_Transform.z;
				march_data[f*4*u_VPort.x*u_VPort.y + i * u_VPort.y + j].w = l_Distance;
					
				//march_data[l_Counter] = l_Distance;	
			}
		}
	}
}