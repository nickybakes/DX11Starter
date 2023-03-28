#ifndef SHADER_INCLUDES // Each .hlsli file needs a unique identifier!
#define SHADER_INCLUDES
#define MAX_SPECULAR_EXPONENT 256.0f
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

// ALL of your code pieces (structs, functions, etc.) go here!


struct Light
{
    int Type; // Which kind of light? 0, 1 or 2 (see above)
    float3 Direction; // Directional and Spot lights need a direction
    float Range; // Point and Spot lights have a max range for attenuation
    float3 Position; // Point and Spot lights have a position in space
    float Intensity; // All lights need an intensity
    float3 Color; // All lights need a color
    float SpotFalloff; // Spot lights need a value to define their “cone” size
    float3 Padding; // Purposefully padding to hit the 16-byte boundary

};


// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float3 localPosition : POSITION; // XYZ position
    float3 normal : NORMAL; // DIRECTION
    float3 tangent : TANGENT; // TANGENT DIRECTION
    float2 uv : TEXCOORD; // UV COORDINATE
    
};


// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD; // UV Coordinate
    float3 normal : NORMAL;
    float3 worldPosition : POSITION;
    float3 tangent : TANGENT; // TANGENT DIRECTION
};


struct VertexToPixel_Sky
{
    float4 screenPosition : SV_POSITION;
    float3 sampleDir : DIRECTION; // UV Coordinate
};
#endif