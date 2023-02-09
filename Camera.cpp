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
}

void Camera::UpdateViewMatrix()
{
}
