#version 400

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec3 VertexNormal;
layout (location = 3) in vec3 VertexTangent;
layout (location = 4) in vec3 VertexBitangent;

out VertexData {
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 uv;
} VertexOut;

uniform mat3 NormalMatrix;

void main()
{
    VertexOut.normal = normalize(NormalMatrix * VertexNormal);
    VertexOut.tangent = normalize(NormalMatrix * VertexTangent);
    VertexOut.bitangent = normalize(NormalMatrix * VertexBitangent);
    VertexOut.uv = VertexTexCoord;
    gl_Position = vec4(VertexPosition)/VertexPosition.w;
}