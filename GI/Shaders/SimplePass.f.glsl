#version 430

layout(location = 0) out vec4 out_color;

uniform sampler2D DiffuseTexture;
uniform sampler2D ShadowTexture;
uniform sampler2D RandomTexture;
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
uniform mat3 RotationMatrix;
uniform vec4 LightPosition;
uniform vec3 CameraPosition;

const vec3 lightColor = vec3(150, 150, 150);

const vec4 MaterialAmbientColor  = vec4(0.1, 0.1, 0.1, 0);
const vec3 lightPos = vec3(0,3,0);

void main()
{	
    
	//out_color = vec4(VertexIn.normal,0.0f);

    vec3 n = normalize(VertexIn.normal);
    vec3 lightDir =  vec3(LightPosition-VertexIn.ecPos);
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
                spotEffect = pow(spotEffect, 0.11);
                float att = spotEffect / (1 +
                    0.01 * dist +
                    0.01  * dist * dist);
                
                float fH = tan( 45.0 / 360.0 * 3.14159 ) * 1;
				float fW = fH;

				float fW_Inv = ( 2 * fW );
				float fH_Inv = ( 2 * fH );
				float A_texel = fW_Inv * fH_Inv;

				//vec2 o = (tan_phi_2/float(2048)) * vec2(1.0 + 2 * gl_FragCoord.x - 2048, 1.0 + 2 * gl_FragCoord.y - 2048);
				//float d_ij = sqrt(5);
				float d_ij = sqrt(1);
				//float A = dot(pecs,pecs) * A_texel / (d_ij * d_ij * d_ij * (0.01+ndotl));
				//simplified:

				float A = (A_texel * dist * dist) / (d_ij * d_ij );
				vec3 radiosity = ( d_ij * d_ij * lightColor.rgb ) / ( dist * dist * 3.14159 * 3.14159 * 2 );   
                 
                vec3 halfVec = normalize(lightDir + viewDir);
                NdotHV = max(dot(n,halfVec),0.0);

                out_color = visibility * (vec4(texture( DiffuseTexture, VertexIn.uv ).rgb * (radiosity * 5 ), 0.0f) );
                out_color *= att * pow(NdotHV, 2);

            } else {
                out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb * 0 , 0.0f);
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
    //out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb * NdotL, 0.0f);
    //out_color += texture( ShadowTexture, VertexIn.shadow_coords.xy );
    //out_color = vec4(texture( DiffuseTexture, texture( RandomTexture, VertexIn.uv ).xy ).rgb , 0.0f);
}