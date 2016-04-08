#version 430
#extension GL_ARB_compute_variable_group_size : enable

uniform ivec3 u_Dim;
uniform ivec2 u_VPort;
uniform vec3 u_GridSize;
uniform vec3 u_VoxelSize;

layout(std430, binding=4) readonly buffer DistanceField {
    float field_data[];
};

layout(std430, binding=5) writeonly buffer Marcher {
    float march_data[];
};

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

void main() {
	vec3 l_GlobalTestPosition(0, -10, 0);
	vec2 l_PixelNDC;

	for ( int f = 0; f < 6; f++ ) {
		for ( int i = 0; i < u_VPort.x; i++ ) {
			l_PixelNDC.x = ( i + 0.5) / u_VPort.x;
			for ( int j = 0; j < u_VPort.y; j++ ) {
				l_PixelNDC.y = ( j + 0.5) / u_VPort.y;

				vec3 l_Dir;

				l_Dir = cubeMapPixelToDirection(l_PixelNDC, f);
			}
		}
	}
}