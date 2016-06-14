#version 400

layout (location = 0) in vec4 VertexPosition;
layout (location = 1) in vec2 VertexTexCoord;
layout (location = 2) in vec3 VertexNormal;
layout (location = 3) in vec3 VertexTangent;
layout (location = 4) in vec3 VertexBitangent;

out VertexData {
    vec3 world_pos;
    vec4 ecPos;
    vec4 shadow_coords;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 uv;
} VertexOut;

uniform mat3 NormalMatrix;
uniform mat4 ModelViewMatrix;
uniform mat3 RotationMatrix;
uniform mat4 ModelMatrix;
uniform vec4 LightPosition;
uniform vec3 CameraPosition;

void main()
{
    VertexOut.normal = normalize(NormalMatrix * VertexNormal);
    VertexOut.tangent = normalize(VertexNormal);
    VertexOut.bitangent = normalize(NormalMatrix * VertexNormal);
    //VertexOut.NdotL = max(dot(VertexOut.normal, normalize( LightPosition - vec3(100, -50.0f, -100.0f) )), 0.0);
    VertexOut.uv = VertexTexCoord;
    VertexOut.shadow_coords = VertexPosition;
    VertexOut.ecPos = VertexPosition;
    VertexOut.world_pos = vec3(ModelMatrix * VertexPosition);
    //VertexOut.world_pos = vec3(vec4(VertexPosition)/VertexPosition.w);

    gl_Position = vec4(VertexPosition);
}