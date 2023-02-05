#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "BufferStructs.h"


class Entity
{

public:
	Entity(std::shared_ptr<Mesh> _mesh);
	~Entity();

	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Transform> GetTransform();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Buffer> vsConstantBuffer, VertexShaderExternalData vsData);
private:

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Transform> transform;
};

