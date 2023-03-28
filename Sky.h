#pragma once
#include "DXCore.h"
#include <DirectXMath.h>
#include "d3d11.h"
#include <wrl/client.h>
#include <memory>
#include "SimpleShader.h"
#include "Mesh.h"
#include "Camera.h"
#include <unordered_map>
#include "WICTextureLoader.h"


using namespace DirectX;

class Sky
{
public:

	Sky(std::shared_ptr<Mesh> mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const std::wstring& relativeFolderPath);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);


private:

	// --------------------------------------------------------
// Author: Chris Cascioli
// Purpose: Creates a cube map on the GPU from 6 individual textures
// 
// - You are allowed to directly copy/paste this into your code base
//   for assignments, given that you clearly cite that this is not
//   code of your own design.
//
// - Note: This code assumes you’re putting the function in Sky.cpp, 
//   you’ve included WICTextureLoader.h and you have an ID3D11Device 
//   ComPtr called “device”.  Make any adjustments necessary for
//   your own implementation.
// --------------------------------------------------------


// --- HEADER ---

// Helper for creating a cubemap from 6 individual textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<	ID3D11DeviceContext> context;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler; //for sampler options
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv; //for the cube map texture’s SRV
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencil; //for adjusting the depth buffer comparison type
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer; //for rasterizer options(drawing the object’s “inside”)
	std::shared_ptr<Mesh> mesh; //for the geometry to use when drawing the sky
	std::shared_ptr<SimplePixelShader> ps; //for the sky - specific pixel shader
	std::shared_ptr<SimpleVertexShader> vs; //for the sky - specific vertex shader




};

