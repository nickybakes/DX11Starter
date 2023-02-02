#pragma once
#include <DirectXMath.h>
class Transform
{

public:




private:

	DirectX::XMFLOAT3 position;

	/// <summary>
	/// A rotation quaternion
	/// </summary>
	DirectX::XMFLOAT4 rotation;

	DirectX::XMFLOAT3 scale;


	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldInverseTranspose;



};

