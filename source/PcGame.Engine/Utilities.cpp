#include "Utilities.h"

#include "KeyboardInputHandler.h"

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

void PcGame::Engine::UpdateConstantBuffer(ComPtr<ID3D12Resource> constantBuffer, void* data, size_t size)
{
	void* pData;
	CD3DX12_RANGE readRange(0, 0);
	PcGame::Engine::ThrowOnFail(constantBuffer->Map(0, &readRange, &pData));
	memcpy(pData, data, size);
	constantBuffer->Unmap(0, nullptr);
}

float _acceleration = 0.01;
float _xSpeed = 0;
float _ySpeed = 0;
float _zSpeed = 0;
float _maxSpeed = 0.2f;

float _rotationAcceleration = 0.0025;
float _pitchSpeed = 0;
float _yawSpeed = 0;
float _maxRotationSpeed = 0.05f;

static float Accelerate(PcGame::Engine::KeyboardInputHandler* keyboard, PcGame::Engine::Keys increase, PcGame::Engine::Keys decrease, float value, float rate, float max)
{
	if (keyboard->IsKeyDown(increase))
	{
		value += rate;
	}
	else if (keyboard->IsKeyDown(decrease))
	{
		value -= rate;
	}
	else
	{
		value /= 1.25f;

		if (value < 0.0001f && value > -0.0001f)
		{
			value = 0;
		}
	}

	if (value > max)
	{
		value = max;
	}
	else if (value < -max)
	{
		value = -max;
	}

	return value;
}

void PcGame::Engine::MoveCamera(ICamera* camera)
{
	auto keyboard = KeyboardInputHandler::GetInstance();

	_xSpeed = Accelerate(keyboard, Keys::D, Keys::A, _xSpeed, _acceleration, _maxSpeed);
	_zSpeed = Accelerate(keyboard, Keys::W, Keys::S, _zSpeed, _acceleration, _maxSpeed);
	_ySpeed = Accelerate(keyboard, Keys::Spacebar, Keys::LeftControl, _ySpeed, _acceleration, _maxSpeed);

	_pitchSpeed = Accelerate(keyboard, Keys::UpArrow, Keys::DownArrow, _pitchSpeed, _rotationAcceleration, _maxRotationSpeed);
	_yawSpeed = Accelerate(keyboard, Keys::RightArrow, Keys::LeftArrow, _yawSpeed, _rotationAcceleration, _maxRotationSpeed);

	camera->Move(_xSpeed, _ySpeed, _zSpeed);
	camera->Rotate(_pitchSpeed, _yawSpeed);
}