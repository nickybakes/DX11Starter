#pragma once
#include <DirectXMath.h>
using namespace DirectX;

class Transform
{

public:
	Transform();

	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(float pitch, float yaw, float roll);
	void SetRotation(DirectX::XMFLOAT3 rotation);
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);

	DirectX::XMFLOAT3 GetPosition();
	float* GetPositionPointer();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetForward();

	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();

	void MoveAbsolute(float x, float y, float z);
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);



private:

	void UpdateMatrices();

	DirectX::XMFLOAT3 position;

	DirectX::XMFLOAT3 rotation;

	DirectX::XMFLOAT3 scale;


	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;

	bool worldMatrixDirty;


};

