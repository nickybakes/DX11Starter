#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> _vertexShader, std::shared_ptr<SimplePixelShader> _pixelShader, XMFLOAT4 _colorTint, float _roughness)
{
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	colorTint = _colorTint;
	roughness = _roughness;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return pixelShader;
}

XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

float Material::GetRoughness()
{
	return roughness;
}

void Material::SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader)
{
	vertexShader = _vertexShader;
}

void Material::SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader)
{
	pixelShader = _pixelShader;
}

void Material::SetColorTint(XMFLOAT4 _colorTint)
{
	colorTint = _colorTint;
}

void Material::SetRoughness(float _roughness)
{
	this->roughness = _roughness;
}
