#version 400

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec3 VertexNormal;

out VertexData {
    vec3 normal;
    //vec2 uv;
} VertexOut;

uniform mat3 NormalMatrix;

void main()
{
    VertexOut.normal = normalize(NormalMatrix * VertexNormal);
    //VertexOut.uv = VertexTexCoord;
    gl_Position = vec4(VertexPosition)/VertexPosition.w;
}