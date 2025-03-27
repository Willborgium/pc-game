#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3d12.h>
#include <dxgi1_6.h>
#include <stdint.h>

#include "IState.h"

namespace PcGame::Engine
{
	class Renderer
	{
	public:
		Renderer();
		void Initialize(HWND hwnd, uint32_t width, uint32_t height);
		void UpdateRenderTargetViews();
		void Render(IState* state);
		void Uninitialize();

		ComPtr<ID3D12Resource> CreateConstantBuffer(size_t bufferSize);
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT count, D3D12_DESCRIPTOR_HEAP_TYPE type);
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(UINT count, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags);

		// These methods are just direct maps to DX12 methods, but they can be improved later
		void CreateShaderResourceView(ComPtr<ID3D12Resource> resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* desc, D3D12_CPU_DESCRIPTOR_HANDLE handle);
		ComPtr<ID3D12Resource> CreateCommittedResource(const D3D12_HEAP_PROPERTIES* heapProperties, const D3D12_RESOURCE_DESC* resourceDesc, D3D12_RESOURCE_STATES initialState);
		void Update(ID3D12Resource* destination, ID3D12Resource* intermediate, uint64_t intermediateOffset, uint32_t firstSubresource, uint32_t subresourceCount, const D3D12_SUBRESOURCE_DATA* subresourceData);

		void ResourceBarrier(ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	private:

		void Flush();

		const static unsigned char FrameCount = 3;
		ComPtr<ID3D12Device2> _device;
		ComPtr<ID3D12Debug5> _debugInterface;
		ComPtr<ID3D12CommandQueue> _commandQueue;
		ComPtr<IDXGISwapChain4> _swapChain;
		ComPtr<ID3D12Resource> _depthStencilBuffer;
		ComPtr<ID3D12DescriptorHeap> _dsvDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> _rtvDescriptorHeap;
		ComPtr<ID3D12Resource> _backBuffers[FrameCount];
		ComPtr<ID3D12CommandAllocator> _commandAllocators[FrameCount];
		ComPtr<ID3D12GraphicsCommandList> _commandLists[FrameCount];
		ComPtr<ID3D12RootSignature> _rootSignature;
		ComPtr<ID3D12PipelineState> _pipelineState;
		ComPtr<ID3D12CommandAllocator> _bundleAllocator;

		unsigned int _rtvDescriptorSize;
		unsigned int _currentFrameIndex;
		ComPtr<ID3D12Fence> _fence;
		uint64_t _fenceValue;
		HANDLE _fenceEvent;
		D3D12_VIEWPORT _viewport;
		D3D12_RECT _scissorRect;
	};
};
