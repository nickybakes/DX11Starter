#include "Camera.h"

Camera::Camera(XMFLOAT3 position, XMFLOAT3 rotation, float aspectRatio, float fov, float nearClip, float farClip, float moveSpeed, float mouseSensitivity, bool perspective)
{
	transform.SetPosition(position);
	transform.SetRotation(rotation);
	this->fov = fov;
	this->nearClip = nearClip;
	this->farClip = farClip;
	this->moveSpeed = moveSpeed;
	this->mouseSensitivity = mouseSensitivity;
	this->perspective = perspective;

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

void Camera::Update(float dt)
{
	Input& input = Input::GetInstance();

	float currentSpeed = moveSpeed;

	if (input.KeyDown(VK_SHIFT))
	{ /* Shift is down */
		currentSpeed *= 2;
	}
	if (input.KeyDown(VK_MENU))
	{ /* Control is down */
		currentSpeed *= .5f;
	}


	if (input.KeyDown('W'))
	{
		transform.MoveRelative(0, 0, currentSpeed * dt);
	}
	if (input.KeyDown('S'))
	{
		transform.MoveRelative(0, 0, -currentSpeed * dt);
	}

	if (input.KeyDown('D'))
	{
		transform.MoveRelative(currentSpeed * dt, 0, 0);
	}
	if (input.KeyDown('A'))
	{
		transform.MoveRelative(-currentSpeed * dt, 0, 0);
	}

	if (input.KeyDown('Q'))
	{
		transform.MoveRelative(0, currentSpeed * dt, 0);
	}
	if (input.KeyDown('E'))
	{
		transform.MoveRelative(0, -currentSpeed * dt, 0);
	}

	if (input.KeyDown(VK_SPACE))
	{
		transform.MoveAbsolute(0, currentSpeed * dt, 0);
	}
	if (input.KeyDown('X'))
	{
		transform.MoveAbsolute(0, -currentSpeed * dt, 0);
	}

	if (input.MouseLeftDown() || input.MouseRightDown())
	{
		int cursorMovementX = input.GetMouseXDelta();
		int cursorMovementY = input.GetMouseYDelta();
		/* Other mouse movement code here */

		transform.Rotate(cursorMovementY * mouseSensitivity, cursorMovementX * mouseSensitivity, 0);

		transform.SetRotation(max(-XM_PIDIV2, min(transform.GetPitchYawRoll().x, XM_PIDIV2)), transform.GetPitchYawRoll().y, transform.GetPitchYawRoll().z);
	}

	UpdateViewMatrix();
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(fov, aspectRatio, nearClip, farClip));
}

float* Camera::GetFOVPointer()
{
	return &fov;
}

Transform Camera::GetTransform()
{
	return transform;
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 pos = transform.GetPosition();
	XMFLOAT3 forward = transform.GetForward();
	XMFLOAT3 up = transform.GetUp();
	XMStoreFloat4x4(&viewMatrix, XMMatrixLookToLH(XMLoadFloat3(&pos), XMLoadFloat3(&forward), XMLoadFloat3(&up)));
}
