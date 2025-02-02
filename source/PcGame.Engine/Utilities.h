#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace PcGame::Engine
{
	void ThrowOnFail(HRESULT result);
	void DisplayMessage(LPCWSTR title, LPCWSTR message);
	ComPtr<ID3D12Resource> CreateConstantBuffer(ComPtr<ID3D12Device2> device, size_t bufferSize);
	void UpdateConstantBuffer(ComPtr<ID3D12Resource> constantBuffer, void* data, size_t size);
}