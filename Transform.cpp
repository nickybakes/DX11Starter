#include "Transform.h"
using namespace DirectX;

Transform::Transform() {
	position = XMFLOAT3(0, 0, 0);
	rotation = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
	worldMatrixDirty = false;
}

void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	worldMatrixDirty = true;
}

void Transform::SetPosition(XMFLOAT3 position)
{
	this->position = position;
	worldMatrixDirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation = XMFLOAT3(pitch, yaw, roll);
	worldMatrixDirty = true;
}

void Transform::SetRotation(XMFLOAT3 rotation)
{
	this->rotation = rotation;
	worldMatrixDirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	worldMatrixDirty = true;
}

void Transform::SetScale(XMFLOAT3 scale)
{
	this->scale = scale;
	worldMatrixDirty = true;
}

XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT3 Transform::GetPitchYawRoll()
{
	return rotation;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (worldMatrixDirty) {
		UpdateMatrices();
	}
	return worldMatrix;
}

XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (worldMatrixDirty) {
		UpdateMatrices();
	}
	return worldInverseTransposeMatrix;
}

void Transform::UpdateMatrices() {
	worldMatrixDirty = false;
	XMMATRIX positionMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX world = scaleMatrix * rotationMatrix * positionMatrix;

	XMStoreFloat4x4(&worldMatrix, world);
	XMStoreFloat4x4(&worldInverseTransposeMatrix,
		XMMatrixInverse(0, XMMatrixTranspose(world)));
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position = XMFLOAT3(position.x + x, position.y + y, position.z + z);
	worldMatrixDirty = true;
}

void Transform::MoveAbsolute(XMFLOAT3 offset)
{
	position = XMFLOAT3(position.x + offset.x, position.y + offset.y, position.z + offset.z);
	worldMatrixDirty = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation = XMFLOAT3(rotation.x + pitch, rotation.y + yaw, rotation.z + roll);
	worldMatrixDirty = true;
}

void Transform::Rotate(XMFLOAT3 rotation)
{
	this->rotation = XMFLOAT3(this->rotation.x + rotation.x, this->rotation.y + rotation.y, this->rotation.z + rotation.z);
	worldMatrixDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale = XMFLOAT3(scale.x * x, scale.y * y, scale.z * z);
	worldMatrixDirty = true;
}

void Transform::Scale(XMFLOAT3 scale)
{
	this->scale = XMFLOAT3(this->scale.x * scale.x, this->scale.y * scale.y, this->scale.z * scale.z);
	worldMatrixDirty = true;
}
