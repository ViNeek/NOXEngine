#version 430
#extension GL_ARB_geometry_shader4 : enable

layout(std430, binding=2) readonly buffer VoxelData{
    uint voxel_data[];
};

layout(triangles) in;

layout(triangle_strip, max_vertices = 1048) out;

/*

*/
/*
syntax error, unexpected identifier, expecting reserved word or reserved word "in" or reserved word "out" or reserved word "uniform" at token "buffer"
*/

uniform uvec3 u_Dim;
uniform uvec3 GridSize;
uniform vec3 GridMin;
uniform vec3 VoxelSize;

//uniform mat3 NormalMatrix;
uniform mat4 MVP;
//uniform mat4 depthMVP;

in VertexData {
    vec3 normal;
	vec3 worldcoord;
    //vec2 uv;
} VertexIn[];
 
out VertexData {
    vec3 normal;
} VertexOut;

uint getVoxelAt( uint x, uint y, uint z ) {
	return voxel_data[x * u_Dim.x * u_Dim.y + y * u_Dim.x + z];
}

// Generate a sphere at the specified position
void sphere(int longitude_steps, int latitude_steps, vec4 position) {
    const float PI = 3.141592f;

	if (longitude_steps < 2) longitude_steps = 2;
	if (latitude_steps < 4) latitude_steps = 4;

	float sphere_radius = length(VoxelSize);
	float phi_step = 2 * PI / float(latitude_steps);
	float theta_step = PI / float(longitude_steps);
	float phi = 0;
	float theta = 0;

	for (int j = 0; j < longitude_steps; j++)
	{
		// temp variables to avoid calculating trigonometric values many times
		float costheta = cos(theta);
		float sintheta = sin(theta);
		float costheta_plus_step = cos(theta + theta_step);
		float sintheta_plus_step = sin(theta + theta_step);

		phi = 0;
		for (int i = 0; i < latitude_steps; i++)
		{
			// temp variables to avoid calculating trigonometric values many times
			float cosphi = cos(phi);
			float sinphi = sin(phi);
			float cosphi_plus_step = cos(phi + phi_step);
			float sinphi_plus_step = sin(phi + phi_step);

			vec4 top_left_vertex;
			top_left_vertex.x = sphere_radius * sintheta * cosphi;
			top_left_vertex.z = sphere_radius * sintheta * sinphi;
			top_left_vertex.y = sphere_radius * costheta;

			vec4 top_right_vertex;
			top_right_vertex.x = sphere_radius * sintheta * cosphi_plus_step;
			top_right_vertex.z = sphere_radius * sintheta * sinphi_plus_step;
			top_right_vertex.y = sphere_radius * costheta;

			vec4 bottom_left_vertex;
			bottom_left_vertex.x = sphere_radius * sintheta_plus_step * cosphi;
			bottom_left_vertex.z = sphere_radius * sintheta_plus_step * sinphi;
			bottom_left_vertex.y = sphere_radius * costheta_plus_step;

			vec4 bottom_right_vertex;
			bottom_right_vertex.x = sphere_radius * sintheta_plus_step * cosphi_plus_step;
			bottom_right_vertex.z = sphere_radius * sintheta_plus_step * sinphi_plus_step;
			bottom_right_vertex.y = sphere_radius * costheta_plus_step;

            // Emit Vertex

            gl_Position = MVP *  (bottom_left_vertex + position);
		    VertexOut.normal = vec3(normalize(bottom_left_vertex));
		    EmitVertex();
            gl_Position = MVP *  ( bottom_right_vertex + position);
            VertexOut.normal = vec3(normalize(bottom_right_vertex));
		    EmitVertex();
            gl_Position = MVP *  (top_right_vertex + position);
            VertexOut.normal = vec3(normalize(top_right_vertex));
		    EmitVertex();

            EndPrimitive();

            gl_Position = MVP *  (top_right_vertex + position);
            VertexOut.normal = vec3(normalize(top_right_vertex));
		    EmitVertex();
            gl_Position = MVP *  (top_left_vertex + position);
            VertexOut.normal = vec3(normalize(top_left_vertex));
		    EmitVertex();
            gl_Position =  MVP *  (bottom_left_vertex + position);
            VertexOut.normal = vec3(normalize(bottom_left_vertex));
		    EmitVertex();

            EndPrimitive();

            /*
			sphere_data.push_back(bottom_left_vertex);
			sphere_data.push_back(bottom_right_vertex);
			sphere_data.push_back(top_right_vertex);
			sphere_data.push_back(top_right_vertex);
			sphere_data.push_back(top_left_vertex);
			sphere_data.push_back(bottom_left_vertex);
            */

			phi += phi_step;
		}

		theta += theta_step;
	}
}

void cube(vec4 position) {
    gl_Position = MVP *  (position + vec4(0,0,1,0));
	//VertexOut.normal = vec3(normalize(bottom_left_vertex));
	EmitVertex();
    gl_Position = MVP *  ( position + (0,1,0,0));
    //VertexOut.normal = vec3(normalize(bottom_right_vertex));
	EmitVertex();
    gl_Position = MVP *  ( position + vec4(1,0,0,0));
    //VertexOut.normal = vec3(normalize(top_right_vertex));
	EmitVertex();

    EndPrimitive();

    gl_Position = MVP *  (position + vec4(1,0,0,0));
    //VertexOut.normal = vec3(normalize(top_right_vertex));
	EmitVertex();
    gl_Position = MVP *  (position + vec4(1,0,1,0));
    //VertexOut.normal = vec3(normalize(top_left_vertex));
	EmitVertex();
    gl_Position =  MVP * (position + vec4(0,1,1,0));
    //VertexOut.normal = vec3(normalize(bottom_left_vertex));
	EmitVertex();

    EndPrimitive();
}

void main()
{
	vec4 pos;
	for(int i = 0; i < gl_VerticesIn; i++)
	{
        vec3 VoxelWorldCoord = ( VertexIn[i].worldcoord - GridMin ) / VoxelSize;
	    ivec3 VoxelGridCoord = ivec3(VoxelWorldCoord);

		pos = MVP * gl_in[i].gl_Position;

        uint l_Occupied = getVoxelAt(VoxelGridCoord.x, VoxelGridCoord.y, VoxelGridCoord.z);


        if ( l_Occupied == 0 ) {
            //sphere(2, 4, vec4(VertexIn[i].worldcoord, 0));
            cube(vec4(VertexIn[i].worldcoord, 0));
        } else {
		    VertexOut.normal = VertexIn[i].normal;
		    gl_Position = pos;

		    //EmitVertex();
       }
	}

	//EndPrimitive();
}