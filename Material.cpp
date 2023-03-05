#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> _vertexShader, std::shared_ptr<SimplePixelShader> _pixelShader, XMFLOAT4 _colorTint, float _roughness)
{
	vertexShader = _vertexShader;
	pixelShader = _pixelShader;
	colorTint = _colorTint;
	roughness = _roughness;
}

void Material::PrepareMaterial()
{
	vertexShader->SetShader();
	pixelShader->SetShader();
	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second); }
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

void Material::AddTextureSRV(std::string key, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ key, srv });
}

void Material::AddSampler(std::string key, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ key, sampler });
}

void Material::SetColorTint(XMFLOAT4 _colorTint)
{
	colorTint = _colorTint;
}

void Material::SetRoughness(float _roughness)
{
	this->roughness = _roughness;
}
