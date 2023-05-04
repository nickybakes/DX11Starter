#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Helpers.h"
#include <memory>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "WICTextureLoader.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#include <DirectXMath.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
	shadowProjectionMatrix = {};
	shadowViewMatrix = {};
	ambientColor = XMFLOAT3(0.0f, 0.0f, 0.0f);
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	CreatePostProcessingResurces(false);
	CalculatePixelSize();

	//create sampler state for textures
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC samplerDescription = {};
	samplerDescription.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescription.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDescription.MaxAnisotropy = 8;
	samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDescription, sampler.GetAddressOf());

	materials = { CreatePBRMaterial(L"bronze", sampler),
		CreatePBRMaterial(L"cobblestone", sampler),
		CreatePBRMaterial(L"floor", sampler),
		CreatePBRMaterial(L"paint", sampler),
		CreatePBRMaterial(L"rough", sampler),
		CreatePBRMaterial(L"scratched", sampler),
		CreatePBRMaterial(L"wood", sampler)
	};

	ambientColor = XMFLOAT3(0.0f, 0.1f, 0.25f);

	Light directionalLight1 = {};

	directionalLight1.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.Direction = XMFLOAT3(0.3f, -0.7f, 1.0f);
	XMStoreFloat3(&directionalLight1.Direction, XMVector3Normalize(XMLoadFloat3(&directionalLight1.Direction)));
	directionalLight1.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	directionalLight1.Intensity = 1.0f;

	Light directionalLight2 = {};

	directionalLight2.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	directionalLight2.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	directionalLight2.Intensity = 0.3f;

	Light directionalLight3 = {};

	directionalLight3.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight3.Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	directionalLight3.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	directionalLight3.Intensity = 1.0f;

	Light pointLight1 = {};

	pointLight1.Type = LIGHT_TYPE_POINT;
	pointLight1.Position = XMFLOAT3(-1.5f, 0.0f, -2.0f);
	pointLight1.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	pointLight1.Intensity = 1.0f;
	pointLight1.Range = 5.0f;

	Light pointLight2 = {};

	pointLight2.Type = LIGHT_TYPE_POINT;
	pointLight2.Position = XMFLOAT3(4.5f, 0.0f, -2.0f);
	pointLight2.Color = XMFLOAT3(1.0f, 1.0f, 0.5f);
	pointLight2.Intensity = 1.0f;
	pointLight2.Range = 5.0f;

	lights = { directionalLight1, directionalLight2, directionalLight3, pointLight1, pointLight2 };

	CreateShadowResources(directionalLight1);


	CreateGeometry();


	sky = make_shared<Sky>(meshes[2], sampler, device, context, L"../../Assets/Textures/Skies/Clouds Blue/");



	cameras = {

		std::make_shared<Camera>(
		XMFLOAT3(0.0f, 0.0f, -6.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		(float)this->windowWidth / this->windowHeight,
		XM_PIDIV2,
		.01f,
		1000.0f,
		5.0f,
		.01f,
		true),

		std::make_shared<Camera>(
		XMFLOAT3(0.0f, -3.0f, -3.0f),
		XMFLOAT3(-.7f, 0.0f, 0.0f),
		(float)this->windowWidth / this->windowHeight,
		XM_PIDIV2,
		.01f,
		1000.0f,
		5.0f,
		.01f,
		true),

		std::make_shared<Camera>(
		XMFLOAT3(0.0f, 3.0f, -3.0f),
		XMFLOAT3(.7f, 0.0f, 0.0f),
		(float)this->windowWidth / this->windowHeight,
		XM_PIDIV2,
		.01f,
		1000.0f,
		5.0f,
		.01f,
		true)
	};



	// Set initial graphics API state
	//  - These settings persist until we change them
	//  - Some of these, like the primitive topology & input layout, probably won't change
	//  - Others, like setting shaders, will need to be moved elsewhere later
	{
		// Tell the input assembler (IA) stage of the pipeline what kind of
		// geometric primitives (points, lines or triangles) we want to draw.  
		// Essentially: "What kind of shape should the GPU draw with our vertices?"
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	}

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();
}

shared_ptr<Material> Game::CreatePBRMaterial(const std::wstring& materialName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler) {
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedo;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalness;

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/PBR/" + materialName + L"_albedo.png").c_str(), 0, albedo.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/PBR/" + materialName + L"_normals.png").c_str(), 0, normal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/PBR/" + materialName + L"_roughness.png").c_str(), 0, roughness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/PBR/" + materialName + L"_metal.png").c_str(), 0, metalness.GetAddressOf());

	shared_ptr<Material> mat = make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1));

	mat->AddSampler("BasicSampler", sampler);
	mat->AddTextureSRV("T_Albedo", albedo);
	mat->AddTextureSRV("T_Normal", normal);
	mat->AddTextureSRV("T_Roughness", roughness);
	mat->AddTextureSRV("T_Metalness", metalness);

	return mat;
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PixelShader.cso").c_str());

	shadowVertexShader = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"ShadowVertexShader.cso").c_str());

	ppVS = std::make_shared<SimpleVertexShader>(device, context,
		FixPath(L"PostProcessVertexShader.cso").c_str());

	ppPS = std::make_shared<SimplePixelShader>(device, context,
		FixPath(L"PostProcessPixelShader.cso").c_str());
}


std::vector<DirectX::XMFLOAT3> positions;
std::vector<DirectX::XMFLOAT3> rotations;
std::vector<DirectX::XMFLOAT3> scales;

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{


	//finally, add the meshes to our vector
	meshes = {
		std::make_shared<Mesh>(FixPath(L"../../Assets/Meshes/sphere.obj").c_str(), device, context),
		std::make_shared<Mesh>(FixPath(L"../../Assets/Meshes/cylinder.obj").c_str(), device, context),
		std::make_shared<Mesh>(FixPath(L"../../Assets/Meshes/cube.obj").c_str(), device, context),
		std::make_shared<Mesh>(FixPath(L"../../Assets/Meshes/torus.obj").c_str(), device, context),
		std::make_shared<Mesh>(FixPath(L"../../Assets/Meshes/helix.obj").c_str(), device, context),
		std::make_shared<Mesh>(FixPath(L"../../Assets/Meshes/quad.obj").c_str(), device, context),
	};

	entities = { std::make_shared<Entity>(meshes[0]),
		std::make_shared<Entity>(meshes[1]),
		std::make_shared<Entity>(meshes[2]),
		std::make_shared<Entity>(meshes[3]),
		std::make_shared<Entity>(meshes[4]),
		std::make_shared<Entity>(meshes[5]),
		std::make_shared<Entity>(meshes[0]),
		std::make_shared<Entity>(meshes[0]),
		std::make_shared<Entity>(meshes[0]),
		std::make_shared<Entity>(meshes[0]),
		std::make_shared<Entity>(meshes[0]),
		std::make_shared<Entity>(meshes[0]),
		std::make_shared<Entity>(meshes[0]),
	};

	entities[0]->SetMaterial(materials[0]);
	entities[1]->SetMaterial(materials[1]);
	entities[2]->SetMaterial(materials[2]);
	entities[3]->SetMaterial(materials[1]);
	entities[4]->SetMaterial(materials[2]);
	entities[5]->SetMaterial(materials[3]);

	for (int i = 6; i < entities.size(); i++) {
		entities[i]->SetMaterial(materials[i - 6]);
	}

	entities[0]->GetTransform()->SetPosition(XMFLOAT3(-6, 0, 0));
	entities[1]->GetTransform()->SetPosition(XMFLOAT3(-3, 0, 0));
	entities[2]->GetTransform()->SetPosition(XMFLOAT3(0, 2, -3));
	entities[3]->GetTransform()->SetPosition(XMFLOAT3(3, 0, 0));
	entities[4]->GetTransform()->SetPosition(XMFLOAT3(6, 0, 0));
	entities[5]->GetTransform()->SetPosition(XMFLOAT3(0, -2, 5));
	entities[5]->GetTransform()->SetScale(XMFLOAT3(10, 1, 10));

	for (int i = 6; i < entities.size(); i++) {
		entities[i]->GetTransform()->SetPosition(XMFLOAT3(-8 + ((i - 6) * 2.5f), 2.5f, 0));
	}

	for (int i = 0; i < entities.size(); i++) {
		positions.push_back(entities[i]->GetTransform()->GetPosition());
		rotations.push_back(entities[i]->GetTransform()->GetPitchYawRoll());
		scales.push_back(entities[i]->GetTransform()->GetScale());
	}
}

int shadowResolution = 2048;
float shadowProjSize = 25;


void Game::CreateShadowResources(Light light)
{
	CreateShadowTextures(false);

	D3D11_RASTERIZER_DESC shadowRasterizerDesc = {};
	shadowRasterizerDesc.CullMode = D3D11_CULL_BACK;
	shadowRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	shadowRasterizerDesc.DepthClipEnable = true;
	shadowRasterizerDesc.DepthBiasClamp = 0.0f;
	shadowRasterizerDesc.DepthBias = 1000;
	shadowRasterizerDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRasterizerDesc, &shadowRasterizer);

	SetShadowDirection(light);
}

void Game::CreateShadowTextures(bool release) {
	if (release) {
		shadowDSV.Get()->Release();
		shadowSRV.Get()->Release();
	}
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowResolution;
	shadowDesc.Height = shadowResolution;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, shadowTexture.GetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDepthStencilDesc = {};
	shadowDepthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDepthStencilDesc.Texture2D.MipSlice = 0;
	shadowDepthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(shadowTexture.Get(), &shadowDepthStencilDesc, shadowDSV.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC shadowSRVDesc = {};
	shadowSRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shadowSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shadowSRVDesc.Texture2D.MipLevels = 1;
	shadowSRVDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture.Get(), &shadowSRVDesc, shadowSRV.GetAddressOf());

	//add sampler to materials only on the first time creating all this stuff
	if (!release) {
		D3D11_SAMPLER_DESC shadowSamplerDesc = {};
		shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		shadowSamplerDesc.BorderColor[0] = 1.0f;
		shadowSamplerDesc.BorderColor[1] = 1.0f;
		shadowSamplerDesc.BorderColor[2] = 1.0f;
		shadowSamplerDesc.BorderColor[3] = 1.0f;

		shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		device->CreateSamplerState(&shadowSamplerDesc, &shadowSampler);

		for (std::shared_ptr<Material> m : materials) {
			m->AddSampler("ShadowSampler", shadowSampler);
		}
	}


	for (std::shared_ptr<Material> m : materials) {
		m->AddTextureSRV("ShadowMap", shadowSRV);
	}
}

void Game::SetShadowDirection(Light light) {
	XMVECTOR lightDirection = XMLoadFloat3(&light.Direction);

	XMMATRIX shadowView = XMMatrixLookAtLH(
		-lightDirection * 20,
		lightDirection,
		XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&shadowViewMatrix, shadowView);

	XMMATRIX lightProjection = XMMatrixOrthographicLH(shadowProjSize, shadowProjSize, 0.1f, 100.0f);
	XMStoreFloat4x4(&shadowProjectionMatrix, lightProjection);
}



int blurRadius;
float pixelWidth;
float pixelHeight;

void Game::CreatePostProcessingResurces(bool remakeTexture)
{
	if (!remakeTexture) {
		// Sampler state for post processing
		D3D11_SAMPLER_DESC ppSampDesc = {};
		ppSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		ppSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		ppSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		ppSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		ppSampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		device->CreateSamplerState(&ppSampDesc, ppSampler.GetAddressOf());
	}

	// Describe the texture we're creating
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = windowWidth;
	textureDesc.Height = windowHeight;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	// Create the resource (no need to track it after the views are created below)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
	device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	device->CreateRenderTargetView(
		ppTexture.Get(),
		&rtvDesc,
		ppRTV.ReleaseAndGetAddressOf());
	// Create the Shader Resource View
	// By passing it a null description for the SRV, we
	// get a "default" SRV that has access to the entire resource
	device->CreateShaderResourceView(
		ppTexture.Get(),
		0,
		ppSRV.ReleaseAndGetAddressOf());
}

void Game::ResetAndRecreatePostProcessingTexture()
{
	ppRTV.Reset();
	ppSRV.Reset();
	CreatePostProcessingResurces(true);
}

void Game::CalculatePixelSize() 
{
	pixelWidth = 1.0f / (float)windowWidth;
	pixelHeight = 1.0f / (float)windowHeight;
}

// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	////loop through our vector of mesh pointers and draw each one!
	for (std::shared_ptr<Camera> cam : cameras)
	{
		cam->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}

	ResetAndRecreatePostProcessingTexture();
	CalculatePixelSize();
}

bool showDemoWindow;
int activeCameraIndex = 0;

void Game::UpdateImGui(float deltaTime, float totalTime)
{
	// Feed fresh input data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// Determine new input capture
	Input& input = Input::GetInstance();
	input.SetKeyboardCapture(io.WantCaptureKeyboard);
	input.SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window
	if (showDemoWindow)
		ImGui::ShowDemoWindow();


	ImGui::Begin("Game Info", nullptr, ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Window Size: %d x %d", windowWidth, windowHeight);
	ImGui::Text("FPS: %.f", ImGui::GetIO().Framerate);

	if (ImGui::Button("Toggle Demo Window"))
	{
		showDemoWindow = !showDemoWindow;
	}

	ImGui::ColorEdit3("Directional Light 1 Color", &lights[0].Color.x);
	ImGui::ColorEdit3("Directional Light 2 Color", &lights[1].Color.x);
	ImGui::ColorEdit3("Directional Light 3 Color", &lights[2].Color.x);
	ImGui::DragFloat3("Point Light 1 Position", &lights[3].Position.x, .1f);
	ImGui::DragFloat3("Point Light 2 Position", &lights[4].Position.x, .1f);

	ImGui::RadioButton("Camera 0", &activeCameraIndex, 0);
	ImGui::RadioButton("Camera 1", &activeCameraIndex, 1);
	ImGui::RadioButton("Camera 2", &activeCameraIndex, 2);

	if (ImGui::DragFloat3("Position", cameras[activeCameraIndex]->GetTransform().GetPositionPointer())) {
		cameras[activeCameraIndex]->UpdateViewMatrix();
	}
	if (ImGui::SliderFloat("FOV", cameras[activeCameraIndex]->GetFOVPointer(), XM_PIDIV4, XM_PIDIV2)) {
		cameras[activeCameraIndex]->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}

	ImGui::SliderInt("Blur Radius", &blurRadius, 0, 16);

	if (ImGui::BeginCombo("Shadow Resolution", std::to_string(shadowResolution).c_str()))
	{
		for (int i = 4; i < 13; i++)
		{
			int currentSquare = (int)pow(2, i);
			const bool is_selected = (shadowResolution == currentSquare);
			if (ImGui::Selectable(std::to_string(currentSquare).c_str(), is_selected)) {
				shadowResolution = currentSquare;
				CreateShadowTextures(true);
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::SliderFloat("Shadow Projection Size", &shadowProjSize, 5.0f, 500.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) {
		SetShadowDirection(lights[0]);
	}

	ImGui::Image(shadowSRV.Get(), ImVec2(128, 128));


	ImGui::NewLine();

	if (ImGui::TreeNode("Entities")) {
		for (int i = 0; i < entities.size(); i++)
		{
			char title[10];
			sprintf_s(title, "Entity %d", i);
			ImGui::PushID(i);
			if (ImGui::TreeNode(title)) {
				ImGui::DragFloat3("Position", &positions[i].x);
				ImGui::DragFloat3("Rotation", &rotations[i].x);
				ImGui::DragFloat3("Scale", &scales[i].x);

				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::TreePop();
	}


	ImGui::End();

}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	UpdateImGui(deltaTime, totalTime);


	cameras[activeCameraIndex]->Update(deltaTime);

	positions[0].z = DirectX::XMScalarSin(totalTime);
	positions[2].x = DirectX::XMScalarSin(totalTime) * 2;
	rotations[3].z = totalTime;
	rotations[3].x = totalTime;
	rotations[4].z = totalTime;

	for (int i = 0; i < entities.size(); i++) {
		entities[i]->GetTransform()->SetPosition(positions[i]);
		entities[i]->GetTransform()->SetRotation(rotations[i]);
		entities[i]->GetTransform()->SetScale(scales[i]);
	}
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	RenderShadows();


	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.4f, 0.6f, 0.75f, 1.0f }; // Cornflower Blue
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

		//PRE RENDER

		context->ClearRenderTargetView(ppRTV.Get(), bgColor);
	}

	//pass in our current total time for this frame
	//this gets used in my custom "hologram" shader
	//pixelShader->SetFloat("totalTime", totalTime);
	pixelShader->SetFloat3("cameraPosition", cameras[activeCameraIndex]->GetTransform().GetPosition());

	pixelShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

	////loop through our vector of mesh pointers and draw each one!
	for (std::shared_ptr<Entity> entity : entities)
	{
		entity->Draw(context, cameras[activeCameraIndex], shadowViewMatrix, shadowProjectionMatrix);
	}

	sky->Draw(context, cameras[activeCameraIndex]);

	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), 0);

	// Activate shaders and bind resources
	// Also set any required cbuffer data (not shown)
	ppVS->SetShader();

	ppPS->SetShader();
	ppPS->SetShaderResourceView("Pixels", ppSRV.Get());
	ppPS->SetSamplerState("ClampSampler", ppSampler.Get());
	ppPS->SetInt("blurRadius", blurRadius);
	ppPS->SetFloat("pixelWidth", pixelWidth);
	ppPS->SetFloat("pixelHeight", pixelHeight);
	ppPS->CopyAllBufferData();
	context->Draw(3, 0); // Draw exactly 3 vertices (one triangle)

	//draw ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;
		swapChain->Present(
			vsyncNecessary ? 1 : 0,
			vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());

		//reset shadow srv's
		ID3D11ShaderResourceView* nullSRVs[128] = {};
		context->PSSetShaderResources(0, 128, nullSRVs);
	}
}


void Game::RenderShadows()
{
	//clear depth
	context->ClearDepthStencilView(shadowDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	ID3D11RenderTargetView* nullRTV{};
	context->OMSetRenderTargets(1, &nullRTV, shadowDSV.Get());

	//set our render's size with a viewport
	D3D11_VIEWPORT viewport = {};
	viewport.Width = (float)shadowResolution;
	viewport.Height = (float)shadowResolution;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	//start renderin' by setting our shader data and looping
	shadowVertexShader->SetShader();
	shadowVertexShader->SetMatrix4x4("view", shadowViewMatrix);
	shadowVertexShader->SetMatrix4x4("projection", shadowProjectionMatrix);

	//set NO pixel shader
	context->PSSetShader(0, 0, 0);

	//set our shadow rasterizer state!

	context->RSSetState(shadowRasterizer.Get());

	for (std::shared_ptr<Entity> entity : entities)
	{
		shadowVertexShader->SetMatrix4x4("world", entity->GetTransform()->GetWorldMatrix());
		shadowVertexShader->CopyAllBufferData();

		entity->GetMesh()->Draw();
	}

	//disable our shadow rasterizer state!
	context->RSSetState(0);

	//reset our render settings for the normal rendering
	viewport.Width = (float)this->windowWidth;
	viewport.Height = (float)this->windowHeight;
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, ppRTV.GetAddressOf(), depthBufferDSV.Get());
}

