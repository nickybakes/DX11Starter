#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
    float4 colorTint;
    float roughness;
    float3 cameraPosition;
    float3 ambientColor;
    Light directionalLight1;
    Light directionalLight2;
    Light directionalLight3;
    Light pointLight1;
    Light pointLight2;

}

float3 DirectionToLight(Light light)
{
    return normalize(-light.Direction.xyz);
}

float3 Diffuse(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}

float3 HandleDirectionalLight(Light light, float3 V, float3 normal)
{
    float3 diffuse = (Diffuse(normal, DirectionToLight(light)) * light.Color * colorTint.rgb);
    
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    if (specExponent < .05f)
    {
        return diffuse;
    }
    
    float3 R = reflect(light.Direction, normal);
    
    float specular = pow(saturate(dot(R, V)), specExponent);
    
    return colorTint.rgb * (diffuse + specular);
}

float3 HandlePointLight(Light light, float3 worldPosition)
{
    float3 direction = normalize(light.Position - worldPosition);
    
    //float3 diffuse = (Diffuse(normal, DirectionToLight(light)) * light.Color * colorTint.rgb);
    
    //float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    //if (specExponent < .05f)
    //{
    //    return diffuse;
    //}
    
    //float3 R = reflect(light.Direction, normal);
    
    //float specular = pow(saturate(dot(R, V)), specExponent);
    
    //return colorTint.rgb * (diffuse + specular);
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
    

    float3 finalLighting = HandleDirectionalLight(directionalLight1, V, input.normal) + 
    HandleDirectionalLight(directionalLight2, V, input.normal) + 
    HandleDirectionalLight(directionalLight3, V, input.normal) +
    (ambientColor * colorTint.rgb);
    
	
    return float4(finalLighting, 1);
}