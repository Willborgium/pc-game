#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdint.h>

namespace PcGame::Engine
{
	class Renderer
	{
	public:
		Renderer();
		void Initialize(HWND hwnd, uint32_t width, uint32_t height);
		void UpdateRenderTargetViews();
		void Render();
		void Uninitialize();

	private:
		const static unsigned char FrameCount = 3;
		ComPtr<ID3D12Device2> _device;
		ComPtr<ID3D12Debug5> _debugInterface;
		ComPtr<ID3D12CommandQueue> _commandQueue;
		ComPtr<IDXGISwapChain4> _swapChain;
		ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap;
		ComPtr<ID3D12Resource> _backBuffers[FrameCount];
		ComPtr<ID3D12PipelineState> _pipelineState;
		ComPtr<ID3D12CommandAllocator> _commandAllocators[FrameCount];
		ComPtr<ID3D12GraphicsCommandList> _commandLists[FrameCount];
		ComPtr<ID3D12CommandAllocator> _bundleAllocator;
		unsigned int _rtvDescriptorSize;
		unsigned int _currentFrameIndex;
		ComPtr<ID3D12Fence> _fence;
		uint64_t _fenceValue;
		uint64_t _frameFenceValues[FrameCount] = {};
		HANDLE _fenceEvent;
	};
};
