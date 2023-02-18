#pragma once

#include <DirectXMath.h>
#include <memory>
#include "SimpleShader.h"


using namespace DirectX;


class Material
{

public:

	Material(std::shared_ptr<SimpleVertexShader> _vertexShader, std::shared_ptr<SimplePixelShader> _pixelShader, XMFLOAT4 _colorTint);

	std::shared_ptr<SimpleVertexShader> GetVertexShader();
	std::shared_ptr<SimplePixelShader> GetPixelShader();

	XMFLOAT4 GetColorTint();

	void SetVertexShader(std::shared_ptr<SimpleVertexShader> _vertexShader);
	void SetPixelShader(std::shared_ptr<SimplePixelShader> _pixelShader);

	void SetColorTint(XMFLOAT4 _colorTint);


private:

	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimplePixelShader> pixelShader;

	XMFLOAT4 colorTint;

};

