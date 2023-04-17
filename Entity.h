#pragma once
#include <memory>
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"
using namespace std;


class Entity
{

public:
	Entity(shared_ptr<Mesh> _mesh);
	~Entity();

	shared_ptr<Mesh> GetMesh();
	shared_ptr<Transform> GetTransform();
	shared_ptr<Material> GetMaterial();
	void SetMaterial(shared_ptr<Material> _material);

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, shared_ptr<Camera> camera, XMFLOAT4X4 shadowViewMatrix, XMFLOAT4X4 shadowProjectionMatrix);
private:

	shared_ptr<Mesh> mesh;
	shared_ptr<Transform> transform;

	shared_ptr<Material> material;
};

