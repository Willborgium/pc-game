#include "FreeCamera.h"

#include "Utilities.h"

using namespace PcGame::Engine;

FreeCamera::FreeCamera(ComPtr<ID3D12Device2> device, float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
	: _position({ 0.0f, 0.0f, 0.0f }),
	_direction({ 0.0f, 0.0f, 1.0f }),
	_up({ 0.0f, 1.0f, 0.0f }),
	_fieldOfView(fieldOfView),
	_aspectRatio(aspectRatio),
	_nearPlane(nearPlane),
	_farPlane(farPlane),
	_constantBuffer(nullptr)
{
	auto size = sizeof(DirectX::XMMATRIX) * 2;
	_constantBuffer = CreateConstantBuffer(device, size);
}

DirectX::XMFLOAT3 FreeCamera::SetPosition(float x, float y, float z)
{
	_position.x = x;
	_position.y = y;
	_position.z = z;
	return _position;
}

DirectX::XMFLOAT3 FreeCamera::Move(float x, float y, float z)
{
	// Load position and direction into XMVECTORs
	auto positionVec = DirectX::XMLoadFloat3(&_position);
	auto directionVec = DirectX::XMLoadFloat3(&_direction);

	// Normalize the direction vector
	directionVec = DirectX::XMVector3Normalize(directionVec);

	// Calculate the right and up vectors
	auto rightVec = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&_up), directionVec);
	rightVec = DirectX::XMVector3Normalize(rightVec);
	auto upVec = DirectX::XMVector3Cross(directionVec, rightVec);
	upVec = DirectX::XMVector3Normalize(upVec);

	// Scale the direction, right, and up vectors by the respective speeds
	auto moveVec = DirectX::XMVectorAdd(
		DirectX::XMVectorScale(directionVec, z),
		DirectX::XMVectorAdd(
			DirectX::XMVectorScale(rightVec, x),
			DirectX::XMVectorScale(upVec, y)
		)
	);

	// Update the position
	positionVec = DirectX::XMVectorAdd(positionVec, moveVec);
	DirectX::XMStoreFloat3(&_position, positionVec);

	return _position;
}

void FreeCamera::Rotate(float pitch, float yaw)
{
	_rotation.x += pitch;
	_rotation.y += yaw;

	_direction.x = sinf(_rotation.y) * cosf(_rotation.x);
	_direction.y = sinf(_rotation.x);
	_direction.z = cosf(_rotation.y) * cosf(_rotation.x);
}

void FreeCamera::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	auto positionV = DirectX::XMLoadFloat3(&_position);
	auto directionV = DirectX::XMLoadFloat3(&_direction);
	auto targetV = DirectX::XMVectorAdd(positionV, directionV);

	auto view = DirectX::XMMatrixLookAtLH(
		positionV,
		targetV,
		DirectX::XMLoadFloat3(&_up)
	);

	auto projection = DirectX::XMMatrixPerspectiveFovLH(_fieldOfView, _aspectRatio, _nearPlane, _farPlane);

	DirectX::XMMATRIX viewProjection[2] = { view, projection };
	UpdateConstantBuffer(_constantBuffer, viewProjection, sizeof(viewProjection));

	commandList->SetGraphicsRootConstantBufferView(0, _constantBuffer->GetGPUVirtualAddress());
}