#include "ShaderIncludes.hlsli"


cbuffer ExternalData : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
}


// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel_Sky main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel_Sky output;

    
    matrix viewNoTranslation = viewMatrix;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    matrix wvp = mul(projectionMatrix, viewNoTranslation);
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.screenPosition.z = output.screenPosition.w;
    
    output.sampleDir = input.localPosition;

    return output;
}