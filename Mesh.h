#pragma once

#include "d3d11.h"
#include <wrl/client.h>
#include <string>
#include "Vertex.h"
#include <fstream>
#include <memory>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;


class Mesh
{

public:
	Mesh(Vertex* vertices, int numberOfVertices, unsigned int* indices, int numberOfIndices, Microsoft::WRL::ComPtr<ID3D11Device> deviceObject, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
	Mesh(const std::wstring& nameOfFile, Microsoft::WRL::ComPtr<ID3D11Device> deviceObject, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
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

	/// <summary>
	/// Helper method for creating direct 3d buffers
	/// </summary>
	void CreateDirect3DBuffer(Vertex* vertices, int numberOfVertices, unsigned int* indices, int numberOfIndices, Microsoft::WRL::ComPtr<ID3D11Device> deviceObject, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);


	// --------------------------------------------------------
// Author: Chris Cascioli
// Purpose: Calculates the tangents of the vertices in a mesh
// 
// - You are allowed to directly copy/paste this into your code base
//   for assignments, given that you clearly cite that this is not
//   code of your own design.
//
// - Code originally adapted from: http://www.terathon.com/code/tangent.html
//   - Updated version now found here: http://foundationsofgameenginedev.com/FGED2-sample.pdf
//   - See listing 7.4 in section 7.5 (page 9 of the PDF)
//
// - Note: For this code to work, your Vertex format must
//         contain an XMFLOAT3 called Tangent
//
// - Be sure to call this BEFORE creating your D3D vertex/index buffers
// --------------------------------------------------------
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	//buffers
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

	//device context for d3D
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	//number of indices that will be in the index buffer
	int numberOfIndices;

};