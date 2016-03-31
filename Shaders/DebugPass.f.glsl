#version 430

layout(location = 0) out vec4 out_color;

in VertexData {
    vec4 color;
} VertexIn;

void main()
{
	out_color = vec4(VertexIn.color);
}