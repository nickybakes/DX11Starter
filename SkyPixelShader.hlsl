#include "ShaderIncludes.hlsli"

TextureCube T_Sky : register(t0); // "t" registers for textures
SamplerState Sampler : register(s0); // "s" registers for samplers

float4 main(VertexToPixel_Sky input) : SV_TARGET
{
    return T_Sky.Sample(Sampler, input.sampleDir);
}