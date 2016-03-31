#version 400
#extension GL_ARB_geometry_shader4 : enable

layout(lines) in;

layout(line_strip, max_vertices = 3) out;

uniform mat4 MVP;

out VertexData {
    vec4 color;
} VertexOut;

void main()
{

	vec4 pos;
	for(int i = 0; i < gl_VerticesIn; i++)
	{
		pos = MVP * gl_in[i].gl_Position;

		VertexOut.color = normalize(abs(gl_in[1].gl_Position - gl_in[0].gl_Position));
		VertexOut.color.w = 0.0f;

		gl_Position = pos;

		EmitVertex();
	}

	EndPrimitive();

}