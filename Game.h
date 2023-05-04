#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <vector>
#include <memory>
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Lights.h"
#include "Sky.h"


class Game
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders();
	shared_ptr<Material> CreatePBRMaterial(const std::wstring& materialName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void CreateGeometry();


	/// <summary>
	/// Creates all shadow related resources for shadows from the light
	/// </summary>
	/// <param name="light">The light you want to cast shadows</param>
	void CreateShadowResources(Light light);

	/// <summary>
	/// Specifically creates the textures (DSV and SRV) for the shadowmaps
	/// </summary>
	/// <param name="release">If true then it releases the old shadow maps. Only make true if you are updating resolution</param>
	void CreateShadowTextures(bool release);

	/// <summary>
	/// Sets the view and projection matrix for the shadow camera from the light
	/// </summary>
	/// <param name="light">The light you want to cast shadows</param>
	void SetShadowDirection(Light light);

	/// <summary>
	/// Sets proper shadow shaders and renders shadows to the depth buffer
	/// </summary>
	void RenderShadows();

	/// <summary>
	/// Creates all resources for post processing
	/// </summary>
	/// <param name="remakeTexture">Make true when recreating PP resources, will only remake the SRV and RTV</param>
	void CreatePostProcessingResurces(bool remakeTexture);

	/// <summary>
	/// Calculates and stores a pixel's width and height in UV space
	/// </summary>
	void CalculatePixelSize();

	/// <summary>
	/// Resets post processing srv, and rtv ComPtrs and then recreates the PP texture wit hnew screen size
	/// </summary>
	void ResetAndRecreatePostProcessingTexture();

	/// <summary>
	/// Updates the ImGui at the beginning of each frame.
	/// </summary>
	void UpdateImGui(float deltaTime, float totalTime);

	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<std::shared_ptr<Entity>> entities;

	std::vector<std::shared_ptr<Camera>> cameras;

	std::shared_ptr<Sky> sky;

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//     Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr

	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;

	std::shared_ptr<SimpleVertexShader> shadowVertexShader;

	XMFLOAT3 ambientColor;

	std::vector<Light> lights;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowDSV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowSRV;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> shadowRasterizer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowSampler;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;


	//needed for post processing
	// Resources that are shared among all post processes
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;
	// Resources that are tied to a particular post process
	std::shared_ptr<SimplePixelShader> ppPS;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; // For rendering
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV; // For sampling


};

