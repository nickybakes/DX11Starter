#pragma once

#include "d3d11.h"
#include <wrl/client.h>
#include "Vertex.h"


class Mesh
{

public:
	Mesh(Vertex* vertices, int numberOfVertices, unsigned int* indices, int numberOfIndices, Microsoft::WRL::ComPtr<ID3D11Device> deviceObject, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	~Mesh();

	/// <summary>
	/// gets the vertex buffer
	/// </summary>
	/// <returns>the vertex buffer for this mesh</returns>
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();

	/// <summary>
	/// gets the index buffer
	/// </summary>
	/// <returns>the index buffer for this mesh</returns>
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();

	/// <summary>
	/// Gets the number of indices in this mesh
	/// </summary>
	/// <returns>The number of indicies in this mesh</returns>
	int GetIndexCount();

	/// <summary>
	/// Draws the mesh to the screen
	/// </summary>
	void Draw();

private:

	//buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	//device context for d3D
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	//number of indices that will be in the index buffer
	int numberOfIndices;

};