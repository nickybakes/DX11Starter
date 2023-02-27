#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
    float totalTime;
}

//The goal of this shader is to look like a hologram
//it will have screenSpace scanlines that animate upwards over time
//I have made this shader using node graph shader tools in Unity and Unreal and want to try making it through code here!
float4 main(VertexToPixel input) : SV_TARGET
{
	//gonna use a sin function with the screenspace Y position to create the initial hologram pattern
    float brightness = abs(sin((input.screenPosition.y + totalTime * 50) / 5));
	//multiplying the value by itself makes the lines smaller/more compact
    brightness *= brightness;
	//lastly, lets add an occasional bright scan line going through
    brightness += 0.3f * frac((input.screenPosition.y + totalTime * 500) / -1000);
	//makes it so the color is never fully black, gives it a glowy feel
    brightness = max(brightness, .3f);
	//then we take the color tint and darken it by the brightness value!
    return float4(colorTint.x * brightness, colorTint.y * brightness, colorTint.z * brightness, 1);
}