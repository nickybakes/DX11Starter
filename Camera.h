#pragma once
#include "Input.h"
#include "Transform.h"
#include <DirectXMath.h>
using namespace DirectX;


class Camera
{

public:

	Camera(XMFLOAT3 position, XMFLOAT3 rotation, float aspectRatio, float fov, float nearClip, float farClip, float moveSpeed, float mouseSensitivity, bool perspective);

	void Update(float dt);

	XMFLOAT4X4 GetViewMatrix();
	XMFLOAT4X4 GetProjectionMatrix();

	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	float* GetFOVPointer();

	Transform GetTransform();

private:

	Transform transform;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	float fov = XM_PIDIV2;
	float nearClip;
	float farClip;
	float moveSpeed;
	float mouseSensitivity;
	bool perspective = true;

};

