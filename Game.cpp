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

	//load textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushion_SRV_D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cushion_SRV_N;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tarp_SRV_D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tarp_SRV_R;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> tarp_SRV_N;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> window_SRV_D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> window_SRV_R;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> window_SRV_N;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brick_SRV_D;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brick_SRV_R;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> brick_SRV_N;

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cushion.png").c_str(), 0, cushion_SRV_D.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cushion_normals.png").c_str(), 0, cushion_SRV_N.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cobblestone.png").c_str(), 0, tarp_SRV_D.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/T_Tarp_01_R.png").c_str(), 0, tarp_SRV_R.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/cobblestone_normals.png").c_str(), 0, tarp_SRV_N.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/rock.png").c_str(), 0, window_SRV_D.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(L"../../Assets/Textures/rock_normals.png").c_str(), 0, window_SRV_N.GetAddressOf());

	//create materials
	shared_ptr<Material> mat0 = make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), .94f);
	shared_ptr<Material> mat1 = make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), .85f);
	shared_ptr<Material> mat2 = make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), .5f);

	mat0->AddSampler("BasicSampler", sampler);
	mat0->AddTextureSRV("T_Diffuse", cushion_SRV_D);
	mat0->AddTextureSRV("T_Roughness", tarp_SRV_R);
	mat0->AddTextureSRV("T_Normal", cushion_SRV_N);

	mat1->AddSampler("BasicSampler", sampler);
	mat1->AddTextureSRV("T_Diffuse", tarp_SRV_D);
	mat1->AddTextureSRV("T_Roughness", tarp_SRV_R);
	mat1->AddTextureSRV("T_Normal", tarp_SRV_N);

	mat2->AddSampler("BasicSampler", sampler);
	mat2->AddTextureSRV("T_Diffuse", window_SRV_D);
	mat2->AddTextureSRV("T_Roughness", window_SRV_R);
	mat2->AddTextureSRV("T_Normal", window_SRV_N);

	materials = { mat0, mat1, mat2};

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


	ambientColor = XMFLOAT3(0.0f, 0.1f, 0.25f);

	Light directionalLight1 = {};

	directionalLight1.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	directionalLight1.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	directionalLight1.Intensity = 1.0f;

	Light directionalLight2 = {};

	directionalLight2.Type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	directionalLight2.Color = XMFLOAT3(1.0f, 1.0f, 1.0f);
	directionalLight2.Intensity = 1.0f;

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
	};

	entities = { std::make_shared<Entity>(meshes[0]), std::make_shared<Entity>(meshes[1]), std::make_shared<Entity>(meshes[2]), std::make_shared<Entity>(meshes[3]), std::make_shared<Entity>(meshes[4])};

	entities[0]->SetMaterial(materials[0]);
	entities[1]->SetMaterial(materials[1]);
	entities[2]->SetMaterial(materials[2]);
	entities[3]->SetMaterial(materials[1]);
	entities[4]->SetMaterial(materials[2]);

	entities[0]->GetTransform()->SetPosition(XMFLOAT3(-6, 0, 0));
	entities[1]->GetTransform()->SetPosition(XMFLOAT3(-3, 0, 0));
	entities[2]->GetTransform()->SetPosition(XMFLOAT3(0, 0, 0));
	entities[3]->GetTransform()->SetPosition(XMFLOAT3(3, 0, 0));
	entities[4]->GetTransform()->SetPosition(XMFLOAT3(6, 0, 0));

	for (int i = 0; i < entities.size(); i++) {
		positions.push_back(entities[i]->GetTransform()->GetPosition());
		rotations.push_back(entities[i]->GetTransform()->GetPitchYawRoll());
		scales.push_back(entities[i]->GetTransform()->GetScale());
	}
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

	ImGui::ColorEdit3("Ambient Color", &ambientColor.x);
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

	//rotations[0].x = totalTime;
	//rotations[1].x = totalTime;
	//rotations[2].x = totalTime;
	//rotations[3].x = totalTime;
	//rotations[4].x = totalTime;
	//rotations[0].y = totalTime;
	//rotations[1].y = totalTime;
	//rotations[2].y = totalTime;
	//rotations[3].y = totalTime;
	//rotations[4].y = totalTime;


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
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		const float bgColor[4] = { 0.4f, 0.6f, 0.75f, 1.0f }; // Cornflower Blue
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	//pass in our current total time for this frame
	//this gets used in my custom "hologram" shader
	//pixelShader->SetFloat("totalTime", totalTime);
	pixelShader->SetFloat3("cameraPosition", cameras[activeCameraIndex]->GetTransform().GetPosition());

	pixelShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());

	////loop through our vector of mesh pointers and draw each one!
	for (std::shared_ptr<Entity> entity : entities)
	{
		entity->GetMaterial()->GetPixelShader()->SetFloat3("ambientColor", ambientColor);
		entity->Draw(context, cameras[activeCameraIndex]);
	}

	sky->Draw(context, cameras[activeCameraIndex]);

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
	}
}