#include "ShaderIncludes.hlsli"

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
float1 Specular(Light light, float3 V, float3 normal)
{
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    if (specExponent < .05f)
    {
        return 0.0f;
    }
    
    float3 R = reflect(light.Direction, normal);
    
    return pow(saturate(dot(R, V)), specExponent);
}

//Calculates all lighting data for a directional light for this pixel
float3 HandleDirectionalLight(Light light, float3 V, float3 normal)
{
    float3 diffuse = (Diffuse(normal, DirectionToLight(light)) * light.Color * colorTint.rgb);
    
    return colorTint.rgb * (diffuse + Specular(light, V, normal));
}

//Calculates the attenuation value for a point light
float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

//Calculates all lighting data for a point light for this pixel
float3 HandlePointLight(Light light, float3 worldPosition, float3 V, float3 normal)
{
    float3 direction = normalize(light.Position - worldPosition);
    
    float3 diffuse = (Diffuse(normal, direction) * light.Color * colorTint.rgb);
    
    return colorTint.rgb * (diffuse + Specular(light, V, normal)) * Attenuate(light, worldPosition);
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
    
    float3 V = normalize(cameraPosition - input.worldPosition);
    
    float3 finalLighting = float3(0.0f, 0.0f, 0.0f);
    
    //loop through our light array and calculate all lighting for this pixel
    for (int i = 0; i < 5; i++)
    {
        switch (lights[i].Type)
        {
            case (LIGHT_TYPE_DIRECTIONAL):
                finalLighting += HandleDirectionalLight(lights[i], V, input.normal);
                break;
            
            case (LIGHT_TYPE_POINT):
                finalLighting += HandlePointLight(lights[i], input.worldPosition, V, input.normal);
                break;

        }
    }

    //finally combine it with our ambient color
    finalLighting += (ambientColor * colorTint.rgb);
    
	
    return float4(finalLighting, 1);
}