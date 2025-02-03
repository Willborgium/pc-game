#include "TargetCamera.h"

#include "Utilities.h"

using namespace PcGame::Engine;

static DirectX::XMFLOAT3 RotatePointAroundOrigin(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target, float yaw, float pitch)
{
	// Convert position and target to XMVECTOR
	auto positionVec = DirectX::XMLoadFloat3(&position);
	auto targetVec = DirectX::XMLoadFloat3(&target);

	// Calculate the vector from the target to the position
	auto directionVec = DirectX::XMVectorSubtract(positionVec, targetVec);

	// Convert to spherical coordinates
	float radius = DirectX::XMVectorGetX(DirectX::XMVector3Length(directionVec));
	float azimuth = atan2f(DirectX::XMVectorGetX(directionVec), DirectX::XMVectorGetZ(directionVec));
	float elevation = asinf(DirectX::XMVectorGetY(directionVec) / radius);

	// Adjust the angles
	azimuth += pitch;
	elevation += yaw;

	// Clamp the elevation to avoid flipping
	const float limit = DirectX::XM_PIDIV2 - 0.01f;
	if (elevation > limit) elevation = limit;
	if (elevation < -limit) elevation = -limit;

	// Convert back to Cartesian coordinates
	directionVec = DirectX::XMVectorSet(
		radius * cosf(elevation) * sinf(azimuth),
		radius * sinf(elevation),
		radius * cosf(elevation) * cosf(azimuth),
		0.0f
	);

	// Update the position
	positionVec = DirectX::XMVectorAdd(targetVec, directionVec);
	DirectX::XMFLOAT3 result = {};
	DirectX::XMStoreFloat3(&result, positionVec);

	return result;
}

TargetCamera::TargetCamera(ComPtr<ID3D12Device2> device, float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
	: _position({ 0.0f, 0.0f, 0.0f }),
	  _target({ 0.0f, 0.0f, 1.0f }),
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

DirectX::XMFLOAT3 TargetCamera::SetPosition(float x, float y, float z)
{
	_position.x = x;
	_position.y = y;
	_position.z = z;
	return _position;
}
DirectX::XMFLOAT3 TargetCamera::SetTarget(float x, float y, float z)
{
	_target.x = x;
	_target.y = y;
	_target.z = z;
	return _target;
}

DirectX::XMFLOAT3 TargetCamera::TranslatePosition(float x, float y, float z)
{
	_position.x += x;
	_position.y += y;
	_position.z += z;

	return _position;
}

DirectX::XMFLOAT3 TargetCamera::TranslateTarget(float x, float y, float z)
{
	_position.x += x;
	_position.y += y;
	_position.z += z;

	return _target;
}

void TargetCamera::Rotate(float pitch, float yaw)
{
	_position = RotatePointAroundOrigin(_position, _target, pitch, yaw);
}

void TargetCamera::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	auto view = DirectX::XMMatrixLookAtLH(
		DirectX::XMLoadFloat3(&_position),
		DirectX::XMLoadFloat3(&_target),
		DirectX::XMLoadFloat3(&_up)
	);

	auto projection = DirectX::XMMatrixPerspectiveFovLH(_fieldOfView, _aspectRatio, _nearPlane, _farPlane);

	DirectX::XMMATRIX viewProjection[2] = { view, projection };
	UpdateConstantBuffer(_constantBuffer, viewProjection, sizeof(viewProjection));

	commandList->SetGraphicsRootConstantBufferView(0, _constantBuffer->GetGPUVirtualAddress());
}