#version 400

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec3 VertexNormal;
layout (location = 3) in vec3 VertexTangent;
layout (location = 4) in vec3 VertexBitangent;

void main()
{
    gl_Position = vec4(VertexPosition)/VertexPosition.w;
}