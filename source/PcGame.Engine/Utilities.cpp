#include "Utilities.h"

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tchar.h>
#include <exception>
#include <d3dx12.h>

void PcGame::Engine::ThrowOnFail(HRESULT result)
{
	if (FAILED(result))
	{
		throw std::exception();
	}
}

void PcGame::Engine::DisplayMessage(LPCWSTR title, LPCWSTR message)
{
	MessageBox(nullptr, title, message, 0);
}

ComPtr<ID3D12Resource> PcGame::Engine::CreateConstantBuffer(ComPtr<ID3D12Device2> device, size_t bufferSize)
{
	ComPtr<ID3D12Resource> constantBuffer;

	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	PcGame::Engine::ThrowOnFail(device->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&bufferDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constantBuffer)));

	return constantBuffer;
}

void PcGame::Engine::UpdateConstantBuffer(ComPtr<ID3D12Resource> constantBuffer, void* data, size_t size)
{
	void* pData;
	CD3DX12_RANGE readRange(0, 0);
	PcGame::Engine::ThrowOnFail(constantBuffer->Map(0, &readRange, &pData));
	memcpy(pData, data, size);
	constantBuffer->Unmap(0, nullptr);
}