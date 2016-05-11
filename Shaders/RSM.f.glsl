#version 430

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_color2;

float atten = 0.8;

uniform vec3 u_LightPos;
uniform vec3 u_LightViewDir;
uniform vec3 u_LightFOV;
uniform ivec2 u_VPort;
uniform float u_ZNear;
uniform float u_ZFar;

uniform sampler2D DiffuseTexture;

const float pi = 3.1415926535897932384626433832795;

const vec3 lightColor = vec3(1.0, 1, 1);

in VertexData {
    vec3 normal;
	vec3 worldpos;
	//flat in uint depth;
	//flat in uvec3 factors;
    vec2 uv;
} VertexIn;

void main()
{
	/*float fH = tan( ( u_LightFOV / 360 ) * pi ) * u_ZNear;
	float fW = fH * u_VPort.x / u_VPort.y;

	float l_Width = 2 * fW;
	float l_Height =  2 * fH;

	float l_PixelWidth = l_Width / u_VPort.x;
	float l_PixelHeight = l_Height / u_VPort.y;

	float l_Area = l_PixelWidth * l_PixelHeight;

	vec3 l_Distance = (worldpos - u_LightPos) * (worldpos - u_LightPos);
	vec3 l_ViewDir = normalize(worldpos - u_LightPos);
	vec3 l_PlaneDir = normalize(u_LightViewDir);
    */
	//out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb*gl_FragCoord.z,0.0f);
	//out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb,0.0f);
	out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb * 0.7 + lightColor * 0.3,0.0f);
	out_color2 = vec4(VertexIn.normal, 1.0f);

}