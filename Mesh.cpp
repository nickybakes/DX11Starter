#include "Mesh.h"
#include "Vertex.h"

Mesh::Mesh(Vertex* vertices, int numberOfVertices, unsigned int* indices, int numberOfIndices, Microsoft::WRL::ComPtr<ID3D11Device> deviceObject, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext)
{
	//create our vertex buffer
	//start by making the description
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex) * numberOfVertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//fill in our vertex data to the gpu
	D3D11_SUBRESOURCE_DATA initialVertexData = {};
	initialVertexData.pSysMem = vertices; // pSysMem = Pointer to System Memory

	//then finally make the buffer on the gpu
	deviceObject->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());

	//now for our index buffer!
	//describe the buffer we want
	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(unsigned int) * numberOfIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

	//fill in our index data for the gpu
	D3D11_SUBRESOURCE_DATA initialIndexData = {};
	initialIndexData.pSysMem = indices; // pSysMem = Pointer to System Memory

	//finally make the index buffer on the gpu!
	deviceObject->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());

	this->numberOfIndices = numberOfIndices;
	this->deviceContext = deviceContext;
}

Mesh::~Mesh()
{
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetVertexBuffer()
{
	return vertexBuffer;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> Mesh::GetIndexBuffer()
{
	return indexBuffer;
}

int Mesh::GetIndexCount()
{
	return numberOfIndices;
}


void Mesh::Draw()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	//put the buffers in the input assembler!
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//actually draw the dang mesh!
	deviceContext->DrawIndexed(numberOfIndices, 0, 0);
}
