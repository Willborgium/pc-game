#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl.h>

using namespace Microsoft::WRL;

namespace PcGame::Engine
{
	class ICamera
	{
	public:
		virtual DirectX::XMFLOAT3 GetPosition() const = 0;
		virtual DirectX::XMFLOAT3 SetPosition(float x, float y, float z) = 0;
		virtual DirectX::XMFLOAT3 Move(float x, float y, float z) = 0;
		virtual void Rotate(float pitch, float yaw) = 0;
		virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList) = 0;
	};
}