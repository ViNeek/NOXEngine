#version 420

layout(location = 0) out vec4 out_color;

in VertexData {
    vec3 normal;
    //vec2 uv;
} VertexIn;

void main()
{
    //vec4 in_color = texture(tex, gl_FragCoord.xy / tex_size);
    //out_color = vec4(1.0f,1.0f,1.0f,1.0f);
	out_color = vec4(VertexIn.normal,0.0f);

}