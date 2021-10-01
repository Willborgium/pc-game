#include "Renderer.h"
#include "Utilities.h"
using namespace PcGame::Engine;


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <d3dx12.h>
#include <chrono>

Renderer::Renderer()
{
	_rtvDescriptorSize = 0;
	_currentBackBufferIndex = 0;
}

void EnableDebugging()
{
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugInterface;
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
	ThrowOnFail(result);
#endif
}

ComPtr<IDXGIFactory4> CreateFactory()
{
	ComPtr<IDXGIFactory4> factory;

	UINT createFactoryFlags = 0;

#if defined(_DEBUG)
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	auto result = CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&factory));
	ThrowOnFail(result);

	return factory;
}

ComPtr<IDXGIAdapter4> GetAdapter(bool useWarp)
{
	auto dxgiFactory = CreateFactory();

	ComPtr<IDXGIAdapter1> dxgiAdapter1;
	ComPtr<IDXGIAdapter4> dxgiAdapter4;

	HRESULT result;

	if (useWarp)
	{
		result = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1));
		ThrowOnFail(result);

		result = dxgiAdapter1.As(&dxgiAdapter4);

		return dxgiAdapter4;
	}

	SIZE_T maxDedicatedVideoMemory = 0;

	for (UINT index = 0; dxgiFactory->EnumAdapters1(index, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; index++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

		if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
			dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory &&
			SUCCEEDED(D3D12CreateDevice(
				dxgiAdapter1.Get(),
				D3D_FEATURE_LEVEL_12_0,
				__uuidof(ID3D12Device),
				nullptr
			)))
		{
			maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
			result = dxgiAdapter1.As(&dxgiAdapter4);
			ThrowOnFail(result);
		}
	}

	return dxgiAdapter4;
}

ComPtr<ID3D12Device2> CreateDevice()
{
	auto adapter = GetAdapter(false);

	ComPtr<ID3D12Device2> device;

	auto result = D3D12CreateDevice(
		adapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device));
	ThrowOnFail(result);

	return device;
}

void EnableInfoQueue(ComPtr<ID3D12Device2> device)
{
#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> infoQueue;
	auto result = device.As(&infoQueue);
	ThrowOnFail(result);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

	//D3D12_MESSAGE_CATEGORY suppressedCategories[] = {};

	D3D12_MESSAGE_SEVERITY suppressedSeverities[] =
	{
		D3D12_MESSAGE_SEVERITY_INFO 
	};

	D3D12_MESSAGE_ID suppressedMessages[] =
	{
		D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
		D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
		D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
	};

	D3D12_INFO_QUEUE_FILTER filter = {};
	//filter.DenyList.NumCategories = _countof(suppressedCategories);
	//filter.DenyList.pCategoryList = suppressedCategories;
	filter.DenyList.NumSeverities = _countof(suppressedSeverities);
	filter.DenyList.pSeverityList = suppressedSeverities;
	filter.DenyList.NumIDs = _countof(suppressedMessages);
	filter.DenyList.pIDList = suppressedMessages;

	result = infoQueue->PushStorageFilter(&filter);
	ThrowOnFail(result);
#endif
}

ComPtr<ID3D12CommandQueue> CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type)
{
	ComPtr<ID3D12CommandQueue> commandQueue;

	D3D12_COMMAND_QUEUE_DESC description = {};

	description.Type = type;
	description.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	description.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	description.NodeMask = 0;

	auto result = device->CreateCommandQueue(&description, IID_PPV_ARGS(&commandQueue));
	ThrowOnFail(result);

	return commandQueue;
}

bool CheckTearingSupport()
{
	ComPtr<IDXGIFactory4> factory4;
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&factory4));
	if (FAILED(result))
	{
		return false;
	}

	ComPtr<IDXGIFactory5> factory5;
	result = factory4.As(&factory5);
	if (FAILED(result))
	{
		return false;
	}

	auto isTearingSupported = false;
	result = factory5->CheckFeatureSupport(
		DXGI_FEATURE_PRESENT_ALLOW_TEARING,
		&isTearingSupported,
		sizeof(isTearingSupported));
	if (FAILED(result))
	{
		return false;
	}

	return isTearingSupported;
}

ComPtr<IDXGISwapChain4> CreateSwapChain(
	uint32_t width,
	uint32_t height,
	uint32_t bufferCount,
	HWND hwnd,
	ComPtr<ID3D12CommandQueue> commandQueue)
{
	DXGI_SWAP_CHAIN_DESC1 description = {};
	description.Width = width;
	description.Height = height;
	description.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	description.Stereo = false;
	description.SampleDesc = { 1, 0 };
	description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	description.BufferCount = bufferCount;
	description.Scaling = DXGI_SCALING_STRETCH;
	description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	description.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	auto isTearingSupported = CheckTearingSupport();
	description.Flags = isTearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

	auto factory = CreateFactory();

	ComPtr<IDXGISwapChain1> swapChain1;
	auto result = factory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hwnd,
		&description,
		nullptr,
		nullptr,
		&swapChain1);
	ThrowOnFail(result);

	ComPtr<IDXGISwapChain4> swapChain4;
	result = swapChain1.As(&swapChain4);
	ThrowOnFail(result);

	return swapChain4;
}

ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(ComPtr
	<ID3D12Device2> device,
	D3D12_DESCRIPTOR_HEAP_TYPE type,
	uint32_t descriptorCount)
{
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	D3D12_DESCRIPTOR_HEAP_DESC description = {};
	description.NumDescriptors = descriptorCount;
	description.Type = type;

	auto result = device->CreateDescriptorHeap(
		&description,
		IID_PPV_ARGS(&descriptorHeap));

	return descriptorHeap;
}

void Renderer::UpdateRenderTargetViews()
{
	auto rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (auto backBufferIndex = 0; backBufferIndex < FrameCount; backBufferIndex++)
	{
		ComPtr<ID3D12Resource> backBuffer;
		auto result = _swapChain->GetBuffer(backBufferIndex, IID_PPV_ARGS(&backBuffer));
		ThrowOnFail(result);
		_device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
		_backBuffers[backBufferIndex] = backBuffer;
		rtvHandle.Offset(rtvDescriptorSize);
	}
}

ComPtr<ID3D12CommandAllocator> CreateCommandAllocator(
	ComPtr<ID3D12Device2> device,
	D3D12_COMMAND_LIST_TYPE type)
{
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	auto result = device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator));
	ThrowOnFail(result);
	return commandAllocator;
}

ComPtr<ID3D12GraphicsCommandList> CreateCommandList(
	ComPtr<ID3D12Device2> device,
	ComPtr<ID3D12CommandAllocator> commandAllocator,
	D3D12_COMMAND_LIST_TYPE type)
{
	ComPtr<ID3D12GraphicsCommandList> commandList;
	auto result = device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
	ThrowOnFail(result);
	result = commandList->Close();
	return commandList;
}

ComPtr<ID3D12Fence> CreateFence(ComPtr<ID3D12Device2> device)
{
	ComPtr<ID3D12Fence> fence;

	auto result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	ThrowOnFail(result);

	return fence;
}

HANDLE CreateEventHandle()
{
	auto fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent && "Failed to create fence event.");
	return fenceEvent;
}

uint64_t Signal(
	ComPtr<ID3D12CommandQueue> commandQueue,
	ComPtr<ID3D12Fence> fence,
	uint64_t fenceValue)
{
	auto fenceValueForSignal = fenceValue + 1;
	auto result = commandQueue->Signal(fence.Get(), fenceValueForSignal);
	return fenceValueForSignal;
}

#ifdef max
#undef max
#endif

void WaitForFenceValue(
	ComPtr<ID3D12Fence> fence,
	uint64_t fenceValue,
	HANDLE fenceEvent,
	std::chrono::milliseconds duration = std::chrono::milliseconds::max())
{
	if (fence->GetCompletedValue() > fenceValue)
	{
		return;
	}

	auto result = fence->SetEventOnCompletion(fenceValue, fenceEvent);
	WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
}

void Flush(
	ComPtr<ID3D12CommandQueue> commandQueue,
	ComPtr<ID3D12Fence> fence,
	uint64_t& fenceValue,
	HANDLE fenceEvent)
{
	auto fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
	WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
}

void Renderer::Initialize(HWND hwnd,
	uint32_t width,
	uint32_t height)
{
	EnableDebugging();

	_device = CreateDevice();

	// Figure out why this is saying interface not supported
	//EnableInfoQueue(_device);

	_commandQueue = CreateCommandQueue(_device, D3D12_COMMAND_LIST_TYPE_DIRECT);

	_swapChain = CreateSwapChain(width, height, FrameCount, hwnd, _commandQueue);

	_currentBackBufferIndex = _swapChain->GetCurrentBackBufferIndex();

	_rtvDescriptorHeap = CreateDescriptorHeap(_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FrameCount);

	UpdateRenderTargetViews();

	for (auto commandAllocatorIndex = 0; commandAllocatorIndex < FrameCount; commandAllocatorIndex++)
	{
		_commandAllocators[commandAllocatorIndex] = CreateCommandAllocator(_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
	}

	_commandList = CreateCommandList(_device, _commandAllocators[_currentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT);

	_fence = CreateFence(_device);
	_fenceEvent = CreateEventHandle();

	// https://www.3dgep.com/learning-directx-12-1/#create-a-fence
}

void Renderer::Uninitialize()
{
	Flush
	(
		_commandQueue,
		_fence,
		_fenceValue,
		_fenceEvent
	);
	CloseHandle(_fenceEvent);
}

void Renderer::Render()
{
	auto commandAllocator = _commandAllocators[_currentBackBufferIndex];
	auto backBuffer = _backBuffers[_currentBackBufferIndex];

	commandAllocator->Reset();
	_commandList->Reset(commandAllocator.Get(), nullptr);

	auto presentToRtvBarrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		backBuffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	_commandList->ResourceBarrier(1, &presentToRtvBarrier);

	float clearColor[] = { .4f, .6f, .9f, 1.0f };
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
	(
		_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		_currentBackBufferIndex,
		_rtvDescriptorSize
	);

	_commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

	auto rtvToPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		backBuffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	_commandList->ResourceBarrier(1, &rtvToPresentBarrier);

	auto result = _commandList->Close();
	ID3D12CommandList* const commandLists[] =
	{
		_commandList.Get()
	};
	_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	auto syncInterval = 1;
	auto presentFlags = 0;// DXGI_PRESENT_ALLOW_TEARING;
	result = _swapChain->Present(syncInterval, presentFlags);
	_currentBackBufferIndex = _swapChain->GetCurrentBackBufferIndex();

	WaitForFenceValue(_fence, _frameFenceValues[_currentBackBufferIndex], _fenceEvent);
}