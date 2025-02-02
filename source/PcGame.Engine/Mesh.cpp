#include "Mesh.h"
#include "Utilities.h"

using namespace PcGame::Engine;

Mesh::Mesh(ComPtr<ID3D12Device> device, const std::vector<VertexPositionColor>& vertices, const std::vector<uint32_t>& indices)
{
	// Create vertex buffer
	const UINT vertexBufferSize = static_cast<UINT>(vertices.size() * sizeof(VertexPositionColor));
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	auto vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
	ThrowOnFail(device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&vertexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_vertexBuffer)));

	// Copy vertex data
	UINT8* pVertexDataBegin = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	ThrowOnFail(_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, vertices.data(), vertexBufferSize);
	_vertexBuffer->Unmap(0, nullptr);

	// Initialize vertex buffer view
	_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
	_vertexBufferView.StrideInBytes = sizeof(VertexPositionColor);
	_vertexBufferView.SizeInBytes = vertexBufferSize;

	// Create index buffer
	const UINT indexBufferSize = static_cast<UINT>(indices.size() * sizeof(uint32_t));
	auto indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
	ThrowOnFail(device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&indexBufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&_indexBuffer)));

	// Copy index data
	UINT8* pIndexDataBegin = nullptr;
	ThrowOnFail(_indexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin)));
	memcpy(pIndexDataBegin, indices.data(), indexBufferSize);
	_indexBuffer->Unmap(0, nullptr);

	// Initialize index buffer view
	_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
	_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	_indexBufferView.SizeInBytes = indexBufferSize;

	_indexCount = static_cast<uint32_t>(indices.size());
}

void Mesh::Draw(ComPtr<ID3D12GraphicsCommandList> commandList) const
{
	commandList->IASetVertexBuffers(0, 1, &_vertexBufferView);
	commandList->IASetIndexBuffer(&_indexBufferView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawIndexedInstanced(_indexCount, 1, 0, 0, 0);
}