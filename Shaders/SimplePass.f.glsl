#version 430

layout(location = 0) out vec4 out_color;

uniform sampler2D DiffuseTexture;

in VertexData {
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 uv;
} VertexIn;

void main()
{	
    
	//out_color = vec4(VertexIn.normal/4,0.0f);
    out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb, 0.0f);
}