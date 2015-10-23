#version 430

layout(location = 0) out vec4 out_color;

in VertexData {
    vec3 normal;
    //vec2 uv;
} VertexIn;

uniform uvec3 dim;

layout (std430, binding=2) buffer VoxelData
{
	uint voxel_data[];
};

void main()
{
	uint dimX = dim.x;
	uint dimY = dim.y;
	uint dimZ = dim.z;

    //vec4 in_color = texture(tex, gl_FragCoord.xy / tex_size);
    //out_color = vec4(1.0f,1.0f,1.0f,1.0f);

	int modifier = gl_SampleID;
	if ( modifier < 0 )
		modifier = -modifier;

	if ( modifier > (128 * 128 * 128) ) 
		modifier = (128 * 128 * 128);

	//voxel_data[0] = 2;

	// Do we need atomicity???
	//atomicOr(voxel_data[0], 3);

	voxel_data[1] = 4;
	voxel_data[0] = 3;
	voxel_data[dimX*dimY*1 + dimY*1 + 1] = 5;
	out_color = vec4(VertexIn.normal,0.0f);

}