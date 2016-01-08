#version 430

layout(location = 0) out vec4 out_color;

in VertexData {
    vec3 normal;
	//flat in uint depth;
	//flat in uvec3 factors;
    //vec2 uv;
} VertexIn;

uniform uvec3 dim;

layout (std430, binding=2) buffer VoxelData
{
	uint voxel_data[];
};

void setVoxelAt(int i, int j, int w) {
	voxel_data[dim.x*dim.y*i + dim.y*j + w] = 1;
	//voxel_data[VertexIn.factors.x*i + VertexIn.factors.y*j + VertexIn.factors.z*w] = 1;
}

void main()
{
	/*
    //vec4 in_color = texture(tex, gl_FragCoord.xy / tex_size);
    //out_color = vec4(1.0f,1.0f,1.0f,1.0f);

	
	int modifier = gl_SampleID;
	if ( modifier < 0 )
		modifier = -modifier;

	if ( modifier > (128 * 128 * 128) ) 
		modifier = (128 * 128 * 128);
	

	float zf = clamp(gl_FragCoord.z,0,1); 
	int x = int(floor(gl_FragCoord.x));
	int y = int(floor(gl_FragCoord.y));
	// get an unsigned int in the range of [1, depth];
	// this is set this way (instead of [0, depth] in order for the bit shifting operations to work
	// and to avoid unnecessary if's 
	int z = int(floor(zf*depth + 0.5));
	// Do we need atomicity???
	//atomicOr(voxel_data[0], 3);

	setVoxelAt(x,y,z);
	//setVoxelAt(9,100,1);
	
	out_color = vec4(VertexIn.normal,0.0f);
	*/

	int modifier = gl_SampleID;
	if ( modifier < 0 )
		modifier = -modifier;

	if ( modifier > (128 * 128 * 128) ) 
		modifier = (128 * 128 * 128);

	// Do we need atomicity???
	//atomicOr(voxel_data[0], 3);

	setVoxelAt(0,0,0);
	//setVoxelAt(9,100,1);
	
	out_color = vec4(VertexIn.normal,0.0f);
}