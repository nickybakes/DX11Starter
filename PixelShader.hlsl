#include "ShaderIncludes.hlsli"

Texture2D T_Albedo : register(t0);
Texture2D T_Normal : register(t1);
Texture2D T_Roughness : register(t2);
Texture2D T_Metalness : register(t3); // "t" registers for textures
SamplerState BasicSampler : register(s0); // "s" registers for samplers



cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float3 cameraPosition;
    
    Light lights[5];

}

//Calculates all lighting data for a directional light for this pixel
float3 HandleDirectionalLight(Light light, float3 camPos, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float metalness, float3 specColor)
{
    float3 toLight = normalize(-light.Direction.xyz);
    float3 toCam = normalize(camPos - worldPos);

    
    // Calculate the light amounts
    float diff = DiffusePBR(normal, toLight);
    float3 F;
    float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specColor, F);
// Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diff, spec, metalness);
// Combine the final diffuse and specular values for this light
    return (balancedDiff * surfaceColor + spec) * light.Intensity * light.Color;
}

//Calculates the attenuation value for a point light
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

//Calculates all lighting data for a point light for this pixel
float3 HandlePointLight(Light light, float3 camPos, float3 worldPos, float3 normal, float3 surfaceColor, float roughness, float metalness, float3 specColor)
{
    float3 toLight = normalize(light.Position - worldPos);
    float3 toCam = normalize(camPos - worldPos);
    
    // Calculate the light amounts
    float diff = DiffusePBR(normal, toLight);
    float3 F;
    float3 spec = MicrofacetBRDF(normal, toLight, toCam, roughness, specColor, F);
// Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diff, spec, metalness);
// Combine the final diffuse and specular values for this light
    return (balancedDiff * surfaceColor + spec) * light.Intensity * light.Color * Attenuate(light, worldPos);
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
    
    float3 surfaceColor = pow(T_Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f) * colorTint.rgb;
    
    float roughness = T_Roughness.Sample(BasicSampler, input.uv).r;
    float metalness = T_Metalness.Sample(BasicSampler, input.uv).r;
    
    float3 specularColor = lerp(F0_NON_METAL, surfaceColor.rgb, metalness);
    

    
    float3 finalLighting = surfaceColor * float3(0, 0, 0);
    
    //loop through our light array and calculate all lighting for this pixel
    for (int i = 0; i < 5; i++)
    {
        Light l = lights[i];
        l.Direction = normalize(l.Direction);
        
        switch (lights[i].Type)
        {
            case (LIGHT_TYPE_DIRECTIONAL):
                finalLighting += HandleDirectionalLight(l, cameraPosition, input.worldPosition, input.normal, surfaceColor, roughness, metalness, specularColor);
                break;
            
            case (LIGHT_TYPE_POINT):
                finalLighting += HandlePointLight(l, cameraPosition, input.worldPosition, input.normal, surfaceColor, roughness, metalness, specularColor);
                break;

        }
    }
    
    return float4(pow(finalLighting, 1.0f / 2.2f), 1);
}