#pragma once

#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <d3d12.h>

namespace PcGame::Engine
{
	class IState
	{
	public:
		virtual bool IsInitialized() = 0;
		virtual void Initialize() = 0;
		virtual void Update() = 0;
		virtual void Render(ComPtr<ID3D12GraphicsCommandList> commandList) = 0;
	};
};