#version 430

layout(location = 0) out vec4 out_color;

uniform sampler2D DiffuseTexture;
uniform sampler2D ShadowTexture;
uniform bool ApplyShadows;

in VertexData {
    vec3 world_pos;
    vec4 shadow_coords;
    vec3 normal;
    float NdotL;
    vec3 tangent;
    vec3 bitangent;
    vec2 uv;
} VertexIn;

uniform mat3 NormalMatrix;
uniform vec3 LightPosition;
uniform vec3 CameraPosition;

const vec4 MaterialAmbientColor  = vec4(0.1, 0.1, 0.1, 0);
const vec3 lightPos = vec3(0,3,0);
const vec3 lightDir = vec3(0,0,-1);

void main()
{	
    
	//out_color = vec4(VertexIn.normal,0.0f);

    vec3 halfVec = normalize(NormalMatrix * (CameraPosition - LightPosition) );
    float NdotHV = max(dot(VertexIn.normal, halfVec),0.0);

    vec3 LightToPixel = normalize(VertexIn.world_pos - lightPos);
    float SpotFactor = dot(LightToPixel, lightDir);

    float visibility = 1;
    if ( ApplyShadows ) {
    vec3 ProjCoords = VertexIn.shadow_coords.xyz / VertexIn.shadow_coords.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
    //if ( (VertexIn.shadow_coords.x / VertexIn.shadow_coords.w <= 1 && VertexIn.shadow_coords.x/ VertexIn.shadow_coords.w >= 0) && (VertexIn.shadow_coords.y/ VertexIn.shadow_coords.w <= 1 && VertexIn.shadow_coords.y/ VertexIn.shadow_coords.w >= 0)) {
    //if (dot(normalize( VertexIn.world_pos - lightPos ), vec3(0,0,-1)) > cos(45.0f)) {
        if ( texture( ShadowTexture, UVCoords ).x < z - 0.01 ){
        //if ( texture( ShadowTexture, VertexIn.shadow_coords.xy / VertexIn.shadow_coords.w ).r > VertexIn.shadow_coords.z / VertexIn.shadow_coords.w + 0.1){
        //if ( texture( ShadowTexture, VertexIn.shadow_coords.xy / VertexIn.shadow_coords.w ).z > gl_FragCoord.z + 0.1){
            visibility = 0;
        }

        out_color = MaterialAmbientColor + visibility * (vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f) + vec4(0.6,0.6, 0.6, 0));
        out_color *= VertexIn.NdotL;
    //}
    } else {
        //out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
        out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
    }
    //out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
    //out_color += texture( ShadowTexture, VertexIn.shadow_coords.xy );
}