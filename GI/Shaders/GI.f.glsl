#version 430

layout(location = 0) out vec4 out_color;

uniform sampler2D DiffuseTexture;
uniform sampler2D ShadowTexture;
uniform bool ApplyShadows;

in VertexData {
    vec3 world_pos;
    vec4 ecPos;
    vec4 shadow_coords;
    vec3 normal;
    vec3 tangent;
    vec3 bitangent;
    vec2 uv;
} VertexIn;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform vec4 LightPosition;
uniform vec3 CameraPosition;

const vec4 MaterialAmbientColor  = vec4(0.1, 0.1, 0.1, 0);
const vec3 lightPos = vec3(0,3,0);

void main()
{	
    
	//out_color = vec4(VertexIn.normal,0.0f);

    vec3 n = normalize(VertexIn.normal);
    vec3 lightDir = vec3(LightPosition-VertexIn.ecPos);
    vec3 viewDir = vec3(-VertexIn.ecPos);
    float dist = length(lightDir);

    float NdotL = max(dot(n,normalize(lightDir)),0.0);

    float visibility = 1;
    if ( ApplyShadows ) {
        vec3 ProjCoords = VertexIn.shadow_coords.xyz / VertexIn.shadow_coords.w;
        vec2 UVCoords;
        UVCoords.x = 0.5 * ProjCoords.x + 0.5;
        UVCoords.y = 0.5 * ProjCoords.y + 0.5;
        float z = 0.5 * ProjCoords.z + 0.5;
        //if ( (VertexIn.shadow_coords.x / VertexIn.shadow_coords.w <= 1 && VertexIn.shadow_coords.x/ VertexIn.shadow_coords.w >= 0) && (VertexIn.shadow_coords.y/ VertexIn.shadow_coords.w <= 1 && VertexIn.shadow_coords.y/ VertexIn.shadow_coords.w >= 0)) {
        //if (dot(normalize( VertexIn.world_pos - lightPos ), vec3(0,0,-1)) > cos(45.0f)) {
        if ( texture( ShadowTexture, UVCoords ).x < z - 0.001 ){
        //if ( texture( ShadowTexture, VertexIn.shadow_coords.xy / VertexIn.shadow_coords.w ).r > VertexIn.shadow_coords.z / VertexIn.shadow_coords.w + 0.1){
        //if ( texture( ShadowTexture, VertexIn.shadow_coords.xy / VertexIn.shadow_coords.w ).z > gl_FragCoord.z + 0.1){
            visibility = 0;
        }

        float NdotHV = 0;
        float spotEffect = 0;
        if (NdotL > 0.0) {
            vec3 lightPosition = NormalMatrix * vec3(LightPosition - vec4(100000, -50000.0f, -100000.0f, 0));
            spotEffect = dot(normalize( vec4(lightPosition,0) ),normalize(vec4(lightDir, 0)));
            if (spotEffect > cos(30 * 3.14159265359 / 180) && visibility > 0) {
                spotEffect = pow(spotEffect, 1);
                float att = spotEffect / (1 +
                    0.01 * dist +
                    0.01  * dist * dist);
                 
                vec3 halfVec = normalize(lightDir + viewDir);
                NdotHV = max(dot(n,halfVec),0.0);

                out_color = visibility * (vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f) + vec4(0.8, 0.8 ,0.8, 0));
                out_color *= att * pow(NdotHV, 2);

            } else {
                out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb * 0.1 , 0.0f);
                out_color *= NdotL;
            }
        } else {
            if ( visibility > 0 )
                out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb * 0  , 0.0f);
        }
        
    //}
    } else {
        //out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
        out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
    }
    //out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
    //out_color += texture( ShadowTexture, VertexIn.shadow_coords.xy );
}