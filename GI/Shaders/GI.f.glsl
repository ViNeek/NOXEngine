#version 430

layout(location = 0) out vec4 out_color;

uniform sampler2D DiffuseTexture;
uniform sampler2D ShadowTexture;
uniform bool ApplyShadows;
uniform uvec3 u_Dim;
uniform ivec2 u_VPort;
uniform vec3 u_GridSize;
uniform vec3 u_VoxelSize;
uniform vec3 u_GridMin;

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
uniform vec3 u_LightDirection;
uniform mat3 RotationMatrix;

layout(std430, binding=5) readonly buffer Marcher {
    vec4 march_data[];
};

layout(std430, binding=6) readonly buffer Index {
    int index_data[];
};

vec3 cubeMapPixelToDirection(vec2 texcoord, int face)
{
        vec3 uv = vec3(texcoord, 1);
        uv.xy = 2.0 * texcoord.xy - 1.0;

        vec3 uv2;
        switch(face)
        {
        case 0:
                //positive x
                uv2 = vec3(1.0, -uv.y, -uv.x);
        break;
        case 1:
                //negative x
                uv2 = vec3(-1.0, -uv.y, uv.x);
        break;
        case 2:
                //positive y
                uv2 = vec3(uv.x, 1.0, uv.y);
        break;
        case 3:
                //negative y
                uv2 = vec3(uv.x, -1.0, uv.y);
        break;
        case 4:
                //positive z
                uv2 = vec3(uv.x, -uv.y, 1.0);
        break;
        case 5:
                //negative z
                uv2 = vec3(-uv.x, -uv.y, -1.0);
        break;
        }
        return normalize(uv2);
}

const vec4 MaterialAmbientColor  = vec4(0.1, 0.1, 0.1, 0);
const vec3 lightPos = vec3(0,3,0);
const vec3 lightColor = vec3(150, 150, 150);

int getIndexAt( int x, int y, int z ) {
	//return field_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z];
	return index_data[x * u_Dim.x * u_Dim.y + u_Dim.x * y + z];
}

int getIndexAt( ivec3 vec ) {
	//return field_data[x + u_Dim.x * y + u_Dim.x * u_Dim.y * z];
	return index_data[vec.x * u_Dim.x * u_Dim.y + u_Dim.x * vec.y + vec.z];
}

vec2 random( vec2 seed )
{
  // We need irrationals for pseudo randomness.
  // Most (all?) known transcendental numbers will (generally) work.
  const vec2 r = vec2(
    23.1406926327792690,  // e^pi (Gelfond's constant)
     2.6651441426902251); // 2^sqrt(2) (Gelfond–Schneider constant)

  float rndNumX = fract( cos( mod( 123456789., 1e-7 + 256. * dot(seed.xy,r) ) ) );
  float rndNumY = fract( cos( mod( 123456789., 1e-7 + 256. * dot(seed.yx,r) ) ) );
  
  return vec2(rndNumX, rndNumY);
}

ivec3 cubeMapSelection(vec3 direction, ivec2 resolution)
{
        ivec3 uv;
        float sc, tc, ma;
        vec3 abs_direction = abs(direction);
        if( abs_direction.x > abs_direction.y)
        {
                if(abs_direction.x > abs_direction.z)
                {
                        sc = -sign(direction.x) * direction.z;
                        tc = -direction.y;
                        ma = direction.x;
                        uv.z = (direction.x > 0.0)? 0 : 1;
                }
                else
                {
                        sc = sign(direction.z) * direction.x;
                        tc = -direction.y;
                        ma = direction.z;
                        uv.z = (direction.z > 0.0)? 4 : 5;
                }
        }
        else
        {
                if(abs_direction.y > abs_direction.z)
                {
                        sc = direction.x;
                        tc = direction.z;
                        ma = direction.y;
                        uv.z = (direction.y > 0.0)? 2 : 3;
                }
                else
                {
                        sc = sign(direction.z) * direction.x;
                        tc = -direction.y;
                        ma = direction.z;
                        uv.z = (direction.z > 0.0)? 4 : 5;
                }
        }

        float s = 0.5 * ( sc / abs(ma) + 1);
        float t = 0.5 * ( tc / abs(ma) + 1);

        s = clamp(s, 0, 1);
        t = clamp(t, 0, 1);

        uv.x = int(resolution.x * s);
        uv.y = int(resolution.y * t);

        return uv;
}

vec4 mc_integrate(int p_Index, vec3 norm) {
    vec4 result = vec4(0,0,0,0);
    //uint l_BufferOffset = u_VPort.x * u_VPort.y * 6 * l_Index;
	ivec2 uniform_cubemap_resolution = ivec2(8,8);
    int l_BufferOffset = uniform_cubemap_resolution.x * uniform_cubemap_resolution.y * 6 * p_Index;

	int wi = uniform_cubemap_resolution.x;
	int he = uniform_cubemap_resolution.y;

	int accepted = 0;

	const float M_PI = 3.14;
	const int probe_size = uniform_cubemap_resolution.x * uniform_cubemap_resolution.y * 6;
	
	/*
        vec3 final_gi = vec3(0);

        const int NUMBER_OF_SAMPLES = 200;
        const float normalization_factor = 1.0 / NUMBER_OF_SAMPLES;

        vec3 w = norm;
        vec3 u = normalize(cross(abs(w.x) > 0.9 ? vec3(0, 1, 0) : vec3(1, 0, 0), w));
        vec3 v = cross(w, u);

        for(int i=0; i<NUMBER_OF_SAMPLES; i++)
        {
                vec3 uv_rand = vec3(0.1 + i*0.1, 0.2 + i*0.2, 0);
                //uv_rand = vec3(0.1 + 0.1, 0.1 + 0.1, 0);
                //vec2 random = texture(random_texture, uv_rand).xy;
                vec2 random = random(uv_rand.xy);
                //random = vec2(1);
                float r1 = 2 * M_PI * random.x; // angle around
                float r2 = random.y;

                vec3 sample_dir = (u * cos(r1) * sqrt(1 - r2) + v * sin(r1) * sqrt(1 - r2) + w * sqrt(r2));
                //sample_dir = normal;
                //sample_dir = norm;
				
                ivec3 coord = cubeMapSelection(sample_dir, uniform_cubemap_resolution);
                //coord = clamp(coord, ivec3(0), ivec3(uniform_cubemap_resolution, 6) - 1);

                //int index = probe_index * probe_size + uniform_cubemap_resolution.x * uniform_cubemap_resolution.y * coord.z + coord.y * uniform_cubemap_resolution.x + coord.x;
				int index = l_BufferOffset + coord.z*wi*he + coord.y * wi + coord.x;

                final_gi += march_data[index].rgb;
        }

        return vec4(normalization_factor * final_gi, 1);
	
	*/
	
    for ( int f = 0; f < 6; f++ ) {
		for ( int i = 0; i < wi; i++ ) {
			for ( int j = 0; j < he; j++ ) {
				vec3 sampleDir = cubeMapPixelToDirection(vec2(i, j) / wi, f );
				float NdotL = max(dot(norm,normalize(sampleDir)),0.0);

				accepted += NdotL > 0 ? 1 : 0;

	            //setVoxelAt(VoxelGridCoord.x + f, VoxelGridCoord.y + i, VoxelGridCoord.z + j);
                result += march_data[l_BufferOffset + f*wi*he + i * wi + j] * NdotL;
	        }
        }
    } 
	

    return result / accepted; 
}

vec4 calculate_gi( vec3 position_grid, vec3 norm ) {
	//vec3 VoxelWorldCoord = ( VertexIn.world_pos - u_GridMin ) / u_VoxelSize;
	//ivec3 VoxelGridCoord = ivec3(VoxelWorldCoord);

	//return mc_integrate(getIndexAt( VoxelGridCoord + ivec3(0,0,0) ), norm);
	vec3 gi_grid_size = vec3(u_Dim);

	vec3 VoxelWorldCoord = ( VertexIn.world_pos - u_GridMin ) / u_VoxelSize;
	ivec3 VoxelGridCoord = ivec3(VoxelWorldCoord);

    vec3 str = position_grid * gi_grid_size - vec3(0.5);
    vec3 new_position_grid = position_grid - 0.5 / gi_grid_size;

	ivec3 new_VoxelGridCoord = ivec3(new_position_grid * gi_grid_size);
	//new_VoxelGridCoord = VoxelGridCoord;

    vec3 fuvw = fract(str);

    int probes_indices[8];
    probes_indices[0] = getIndexAt( new_VoxelGridCoord + ivec3(0,0,0) );
    probes_indices[1] = getIndexAt( new_VoxelGridCoord + ivec3(1,0,0) );
    probes_indices[2] = getIndexAt( new_VoxelGridCoord + ivec3(0,1,0) );
    probes_indices[3] = getIndexAt( new_VoxelGridCoord + ivec3(1,1,0) );
    probes_indices[4] = getIndexAt( new_VoxelGridCoord + ivec3(0,0,1) );
    probes_indices[5] = getIndexAt( new_VoxelGridCoord + ivec3(1,0,1) );
    probes_indices[6] = getIndexAt( new_VoxelGridCoord + ivec3(0,1,1) );
    probes_indices[7] = getIndexAt( new_VoxelGridCoord + ivec3(1,1,1) );

    vec3 sampled_gi[8];
    vec3 final_gi = vec3(0);
	
    for(int i=0; i<8; i++)
    {
		sampled_gi[i] = vec3(0);
        if( probes_indices[i] > -1)
        {
			//sampled_gi[i] = specularSampling(probes_indices[i], p_position, normal, albedo, specularColor, shininess);
			sampled_gi[i] = mc_integrate(probes_indices[i], norm).xyz;
        }
    }
    
	//return vec4(sampled_gi[0], 1);
    //return vec4(position_grid, 1);

	vec3 gi_front = mix(mix(sampled_gi[0], sampled_gi[1], fuvw.x), mix(sampled_gi[2], sampled_gi[3], fuvw.x), fuvw.y);
    vec3 gi_back = mix(mix(sampled_gi[4], sampled_gi[5], fuvw.x), mix(sampled_gi[6], sampled_gi[7], fuvw.x), fuvw.y);
    
	return vec4(mix(gi_front, gi_back, fuvw.z), 1);
	
}

void main()
{	
    
	//out_color = vec4(VertexIn.normal,0.0f);

    vec3 VoxelWorldCoord = ( VertexIn.world_pos - u_GridMin ) / u_VoxelSize;
	ivec3 VoxelGridCoord = ivec3(VoxelWorldCoord);

    vec3 n = normalize(VertexIn.normal);
	vec4 lightP = LightPosition;
    vec3 lightDir = vec3(lightP - VertexIn.ecPos);
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
            //vec3 lightPosition = NormalMatrix * vec3(LightPosition - vec4(100000, -50000.0f, -100000.0f, 0));
            spotEffect = dot(normalize( vec4(-NormalMatrix * u_LightDirection,0) ),normalize(vec4(lightDir, 0)));
            if (spotEffect > cos(30 * 3.14159265359 / 180) && visibility > 0) {
                spotEffect = pow(spotEffect, 1);
                float att = spotEffect / (1 +
                    0.2 * dist +
                    0.2  * dist * dist);
                
				float fH = tan( 45.0 / 360.0 * 3.14159 ) * 1.0;
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
				vec3 radiosity = ( d_ij * d_ij * lightColor.rgb ) / (dist * dist * 3.14159 * 3.14159 * 2 );   
				/*
				float solidA = 4 * asin(fW_Inv * fW_Inv / (d_ij * d_ij));
				radiosity = (lightColor.rgb / solidA)  ;
				*/
				//vec4(radiosity * texture( DiffuseTexture, VertexIn.uv ).rgb, 1.0f);

                vec3 halfVec = normalize(lightDir + viewDir);
                NdotHV = max(dot(n,halfVec),0.0);

                out_color = visibility * (vec4(texture( DiffuseTexture, VertexIn.uv ).rgb * (radiosity * 10) , 0.0f)) ;
                out_color *= pow(NdotHV, 1);

            } else {
                out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb * 0 , 0.0f);
                out_color *= NdotL;
            }
        } else {
            if ( visibility > 0 )
                out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb   , 0.0f) * 0;
        }
        
    //}
    } else {
        //out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
        out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
    }
    //out_color = vec4(texture( DiffuseTexture, VertexIn.uv ).rgb , 0.0f);
    int aIdx = getIndexAt(VoxelGridCoord.x, VoxelGridCoord.y, VoxelGridCoord.z);

	vec3 position_grid = ( VertexIn.world_pos - u_GridMin ) / ( u_GridSize );
    if ( aIdx > 0 )
        //out_color = mc_integrate(aIdx, n);
        out_color += calculate_gi(position_grid, n);
		//out_color = vec4(float(aIdx) / 128.0, float(aIdx) / 128.0, float(aIdx) / 128.0, 1);
	out_color = pow(out_color, vec4(1 / 2.2));
}