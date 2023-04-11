#include "ShaderIncludes.hlsli"

Texture2D T_Diffuse : register(t0); // "t" registers for textures
Texture2D T_Roughness : register(t1); // "t" registers for textures
Texture2D T_Normal : register(t2); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float roughness;
    float3 cameraPosition;
    float3 ambientColor;
    
    Light lights[5];

}

//Gets the normalized direction TOWARD a light's direction
float3 DirectionToLight(Light light)
{
    return normalize(-light.Direction.xyz);
}

//Calculates the diffuse value for this pixel
float3 Diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

//Calculates the specular reflection value for this pixel
float1 Specular(float3 dirToCamera, float3 normal, float3 directionToLight, float roughness, float specularScale)
{
    if (roughness == 1)
        return 0.0f;
    
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    float3 R = reflect(-directionToLight, normal);
    
    return pow(saturate(dot(R, dirToCamera)), specExponent) * specularScale;
}

//Calculates all lighting data for a directional light for this pixel
float3 HandleDirectionalLight(Light light, float3 dirToCamera, float3 normal, float3 surfaceColor, float roughness, float specularScale)
{
    float3 dirToLight = normalize(-light.Direction.xyz);
    float3 diffuse = Diffuse(normal, dirToLight);

    return light.Intensity * light.Color * (diffuse * surfaceColor.rgb + Specular(dirToCamera, normal, dirToLight, roughness, specularScale));
}

//Calculates the attenuation value for a point light
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

//Calculates all lighting data for a point light for this pixel
float3 HandlePointLight(Light light, float3 worldPosition, float3 dirToCamera, float3 normal, float3 surfaceColor, float roughness, float specularScale)
{
    float3 dirToLight = normalize(light.Position - worldPosition);
    
    float3 diffuse = Diffuse(normal, dirToLight);
    
    return light.Intensity * light.Color * (diffuse * surfaceColor.rgb + Specular(dirToCamera, normal, dirToLight, roughness, specularScale)) * Attenuate(light, worldPosition);
}


// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	// Just return the input color
	// - This color (like most values passing through the rasterizer) is 
	//   interpolated for each pixel between the corresponding vertices 
	//   of the triangle we're rendering
	
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    

    //samples the normal map
    float3 unpackedNormal = T_Normal.Sample(BasicSampler, input.uv).rgb * 2.0f - 1.0f;
    
    
    float3 N = input.normal;
    float3 T = normalize(input.tangent - N * dot(input.tangent, N)); // Gram-Schmidt assumes T&N are normalized!
    float3 B = cross(T, N);
    float3x3 TBN = float3x3(T, B, N);
    
    
    input.normal = normalize(mul(unpackedNormal, TBN)); // Note multiplication order!

    
    
    float3 dirToCamera = normalize(cameraPosition - input.worldPosition);
    
    float3 surfaceColor = pow(T_Diffuse.Sample(BasicSampler, input.uv).rgb, 2.2f) * colorTint.rgb;
    
    float specularScale = 1 - T_Roughness.Sample(BasicSampler, input.uv).r;
    
    // Cut the specular if the diffuse contribution is zero
// - any() returns 1 if any component of the param is non-zero
// - In this case, diffuse is a single float value
// - Meaning any() returns 1 if diffuse itself is non-zero
// - In other words:
// - If the diffuse amount is 0, any(diffuse) returns 0
// - If the diffuse amount is != 0, any(diffuse) returns 1
// - So when diffuse is 0, specular becomes 0
    specularScale *= any(surfaceColor);

    
    float3 finalLighting = (ambientColor * surfaceColor);
    
    //loop through our light array and calculate all lighting for this pixel
    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].Type)
        {
            case (LIGHT_TYPE_DIRECTIONAL):
                finalLighting += HandleDirectionalLight(lights[i], dirToCamera, input.normal, surfaceColor, roughness, specularScale);
                break;
            
            case (LIGHT_TYPE_POINT):
                finalLighting += HandlePointLight(lights[i], input.worldPosition, dirToCamera, input.normal, surfaceColor, roughness, specularScale);
                break;

        }
    }
	
    return float4(pow(finalLighting, 1.0f / 2.2f), 1);
}