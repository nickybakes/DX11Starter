#pragma once

#include <DirectXMath.h>
#include <memory>
#include <unordered_map>
#include "SimpleShader.h"


using namespace DirectX;


class Material
{

public:

	Material(std::shared_ptr<SimpleVertexShader> _vertexShader, std::shared_ptr<SimplePixelShader> _pixelShader, XMFLOAT4 _colorTint, float roughness);

	void PrepareMaterial();

	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();

	XMFLOAT4 GetColorTint();
	float GetRoughness();

	void SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader);

	void AddTextureSRV(std::string key, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv);
	void AddSampler(std::string key, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);

	void SetColorTint(XMFLOAT4 _colorTint);
	void SetRoughness(float _roughness);


private:

	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

	XMFLOAT4 colorTint;

	float roughness;

};

