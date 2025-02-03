#pragma once

#include "ICamera.h"

namespace PcGame::Engine
{
	class FreeCamera : public ICamera
	{
	public:
		FreeCamera(ComPtr<ID3D12Device2> device, float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

		DirectX::XMFLOAT3 GetPosition() const { return _position; }
		DirectX::XMFLOAT3 SetPosition(float x, float y, float z);

		DirectX::XMFLOAT3 Move(float x, float y, float z);

		void Rotate(float pitch, float yaw);

		void Render(ComPtr<ID3D12GraphicsCommandList> commandList);
	private:
		DirectX::XMFLOAT3 _position;
		DirectX::XMFLOAT3 _rotation;
		DirectX::XMFLOAT3 _direction;

		const DirectX::XMFLOAT3 _up;
		const float _fieldOfView;
		const float _aspectRatio;
		const float _nearPlane;
		const float _farPlane;

		ComPtr<ID3D12Resource> _constantBuffer;
	};
}