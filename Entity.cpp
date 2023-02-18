#include "Entity.h"

using namespace DirectX;

Entity::Entity(shared_ptr<Mesh> _mesh)
{
	mesh = _mesh;
	transform = make_shared<Transform>();
}

Entity::~Entity()
{
}

shared_ptr<Mesh> Entity::GetMesh()
{
	return mesh;
}

shared_ptr<Transform> Entity::GetTransform()
{
	return transform;
}

shared_ptr<Material> Entity::GetMaterial()
{
	return material;
}

void Entity::SetMaterial(shared_ptr<Material> _material)
{
	material = _material;
}

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, shared_ptr<Camera> camera) {

	material->GetVertexShader()->SetShader();
	material->GetPixelShader()->SetShader();

	std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	vs->SetMatrix4x4("worldMatrix", transform->GetWorldMatrix()); // match variable
	vs->SetMatrix4x4("viewMatrix", camera->GetViewMatrix()); // names in your
	vs->SetMatrix4x4("projectionMatrix", camera->GetProjectionMatrix()); // shader�s cbuffer!

	vs->CopyAllBufferData();

	std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();
	ps->SetFloat4("colorTint", material->GetColorTint()); // Strings here MUST

	ps->CopyAllBufferData();

	mesh->Draw();
}
