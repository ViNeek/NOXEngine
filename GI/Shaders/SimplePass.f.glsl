#version 420

layout(location = 0) out vec4 out_color;

void main()
{
    vec4 in_color = texture(tex, gl_FragCoord.xy / tex_size);
    out_color = vec4(1.0f,1.0f,1.0f,1.0f);
}