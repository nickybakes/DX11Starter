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

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, shared_ptr<Camera> camera, XMFLOAT4X4 shadowViewMatrix, XMFLOAT4X4 shadowProjectionMatrix) {

	material->PrepareMaterial();

	std::shared_ptr<SimpleVertexShader> vs = material->GetVertexShader();
	vs->SetMatrix4x4("worldMatrix", transform->GetWorldMatrix()); // match variable
	vs->SetMatrix4x4("viewMatrix", camera->GetViewMatrix()); // names in your
	vs->SetMatrix4x4("projectionMatrix", camera->GetProjectionMatrix()); // shader’s cbuffer!
	vs->SetMatrix4x4("worldInvTranspose", transform->GetWorldInverseTransposeMatrix());

	vs->SetMatrix4x4("lightView", shadowViewMatrix);
	vs->SetMatrix4x4("lightProjection", shadowProjectionMatrix);

	vs->CopyAllBufferData();

	std::shared_ptr<SimplePixelShader> ps = material->GetPixelShader();
	ps->SetFloat4("colorTint", material->GetColorTint());

	ps->CopyAllBufferData();

	mesh->Draw();
}
