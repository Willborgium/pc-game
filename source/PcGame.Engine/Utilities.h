#pragma once

#include "ICamera.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>

namespace PcGame::Engine
{
	void ThrowOnFail(HRESULT result);
	void DisplayMessage(LPCWSTR title, LPCWSTR message);
	void UpdateConstantBuffer(ComPtr<ID3D12Resource> constantBuffer, void* data, size_t size);

	void MoveCamera(ICamera* camera);
}