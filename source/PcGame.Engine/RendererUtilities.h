#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <chrono>

using namespace Microsoft::WRL;

namespace PcGame::Engine
{
	class RendererUtilities
	{
	public:
		ComPtr<ID3D12Debug5> CreateDebugInterface();
		ComPtr<IDXGIFactory4> CreateFactory();
		ComPtr<IDXGIAdapter4> GetAdapter(ComPtr<IDXGIFactory4> factory, bool useWarp);
		ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIFactory4> factory);
		void EnableInfoQueue(ComPtr<ID3D12Device> device);
		ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		bool CheckTearingSupport();
		ComPtr<IDXGISwapChain4> CreateSwapChain(uint32_t width, uint32_t height, uint32_t bufferCount, HWND hwnd, ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> commandQueue);
		ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount);
		ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		D3D12_SHADER_BYTECODE CreateShaderByteCode(ComPtr<ID3DBlob> shader);
		ComPtr<ID3D12PipelineState> CreatePipelineState(ComPtr<ID3D12Device> device, ComPtr<ID3D12RootSignature> rootSignature);
		ComPtr<ID3D12GraphicsCommandList> CreateCommandList(ComPtr<ID3D12Device2> device, ComPtr<ID3D12CommandAllocator> commandAllocator, ComPtr<ID3D12PipelineState> pipelineState, D3D12_COMMAND_LIST_TYPE type);
		ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> device);
		HANDLE CreateEventHandle();
		void WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration);
		ComPtr<ID3D12RootSignature> CreateRootSignature(ComPtr<ID3D12Device> device);
		void CreateTriangle(ComPtr<ID3D12Device> device, ComPtr<ID3D12Resource>& vertexBuffer, D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);

		void Begin(ComPtr<ID3D12CommandAllocator> commandAllocator, ComPtr<ID3D12GraphicsCommandList> commandList, D3D12_VIEWPORT viewport, D3D12_RECT scissorRect, ComPtr<ID3D12Resource> backBuffer, ComPtr<ID3D12RootSignature> rootSignature, ComPtr<ID3D12PipelineState> pipelineState);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetCurrentRtvDescriptorHandle(ComPtr<ID3D12DescriptorHeap> descriptorHeap, unsigned int currentFrameIndex, unsigned int descriptorSize);
		void Clear(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptorHandle, ComPtr<ID3D12GraphicsCommandList> commandList, float r, float g, float b, float a);
		void Present(ComPtr<ID3D12Resource> backBuffer, ComPtr<ID3D12GraphicsCommandList> commandList, ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<IDXGISwapChain4> swapChain);
	};
}