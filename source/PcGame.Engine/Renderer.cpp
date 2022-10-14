#include "Renderer.h"
#include "Utilities.h"
using namespace PcGame::Engine;


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <d3d12.h>
#include <d3dx12.h>
#include <d3d12sdklayers.h>
#include <chrono>

#define TARGET_FEATURE_LEVEL D3D_FEATURE_LEVEL_12_2

Renderer::Renderer()
{
	_rtvDescriptorSize = 0;
	_currentFrameIndex = 0;
	_fenceEvent = nullptr;
	_fenceValue = 0;
}

ComPtr<ID3D12Debug5> CreateDebugInterface()
{
	ComPtr<ID3D12Debug5> output;
#if defined(_DEBUG)
	auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&output));
	ThrowOnFail(result);
	output->EnableDebugLayer();
#endif
	return output;
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

ComPtr<IDXGIAdapter4> GetAdapter(ComPtr<IDXGIFactory4> factory, bool useWarp)
{
	ComPtr<IDXGIAdapter1> dxgiAdapter1;
	ComPtr<IDXGIAdapter4> dxgiAdapter4;

	HRESULT result;

	if (useWarp)
	{
		result = factory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1));
		ThrowOnFail(result);

		result = dxgiAdapter1.As(&dxgiAdapter4);

		return dxgiAdapter4;
	}

	SIZE_T maxDedicatedVideoMemory = 0;

	for (UINT index = 0; factory->EnumAdapters1(index, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; index++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
		dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

		if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
			dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory &&
			SUCCEEDED(D3D12CreateDevice(
				dxgiAdapter1.Get(),
				TARGET_FEATURE_LEVEL,
				__uuidof(ID3D12Device),
				nullptr
			)))
		{
			maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
			result = dxgiAdapter1.As(&dxgiAdapter4);
			ThrowOnFail(result);
			break;
		}
	}

	return dxgiAdapter4;
}

ComPtr<ID3D12Device2> CreateDevice(ComPtr<IDXGIFactory4> factory)
{
	auto adapter = GetAdapter(factory, false);

	ComPtr<ID3D12Device2> device;

	auto result = D3D12CreateDevice(
		adapter.Get(),
		TARGET_FEATURE_LEVEL,
		IID_PPV_ARGS(&device));
	ThrowOnFail(result);

	return device;
}

void EnableInfoQueue(ComPtr<ID3D12Device> device)
{
#if defined(_DEBUG)
	ComPtr<ID3D12InfoQueue> infoQueue;
	auto result = device.As(&infoQueue);
	ThrowOnFail(result);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_INFO, TRUE);

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
	ComPtr<IDXGIFactory4> factory,
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
	description.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

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

ComPtr<ID3D12PipelineState> CreatePipelineState(ComPtr<ID3D12Device> device)
{
	ComPtr<ID3D12PipelineState> output;

	ComPtr<ID3DBlob> vertexShader;
	ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	auto result = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr);
	ThrowOnFail(result);

	result = D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr);
	ThrowOnFail(result);

	// This should be dynamic and come from the shaders
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDescription = {};

	pipelineStateDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineStateDescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	pipelineStateDescription.DepthStencilState.DepthEnable = FALSE;
	pipelineStateDescription.DepthStencilState.StencilEnable = FALSE;
	pipelineStateDescription.SampleMask = UINT_MAX;
	pipelineStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDescription.NumRenderTargets = 1;
	pipelineStateDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDescription.SampleDesc.Count = 1;

	result = device->CreateGraphicsPipelineState
	(
		&pipelineStateDescription,
		IID_PPV_ARGS(&output)
	);
	ThrowOnFail(result);

	return output;
}

ComPtr<ID3D12GraphicsCommandList> CreateCommandList(
	ComPtr<ID3D12Device2> device,
	ComPtr<ID3D12CommandAllocator> commandAllocator,
	ComPtr<ID3D12PipelineState> pipelineState,
	D3D12_COMMAND_LIST_TYPE type)
{
	ComPtr<ID3D12GraphicsCommandList> commandList;
	
	auto result = device->CreateCommandList(0, type, commandAllocator.Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList));
	ThrowOnFail(result);
	
	result = commandList->Close();
	ThrowOnFail(result);

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
	auto fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		ThrowOnFail(HRESULT_FROM_WIN32(GetLastError()));
	}
	return fenceEvent;
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

void Renderer::Initialize(HWND hwnd,
	uint32_t width,
	uint32_t height)
{
	_debugInterface = CreateDebugInterface();

	auto factory = CreateFactory();

	_device = CreateDevice(factory);

	EnableInfoQueue(_device);

	_commandQueue = CreateCommandQueue(_device, D3D12_COMMAND_LIST_TYPE_DIRECT);

	_swapChain = CreateSwapChain(width, height, FrameCount, hwnd, factory, _commandQueue);

	_currentFrameIndex = _swapChain->GetCurrentBackBufferIndex();

	_rtvDescriptorHeap = CreateDescriptorHeap(_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FrameCount);

	_rtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	UpdateRenderTargetViews();

	_bundleAllocator = CreateCommandAllocator(_device, D3D12_COMMAND_LIST_TYPE_BUNDLE);

	for (auto index = 0; index < FrameCount; index++)
	{
		_commandAllocators[index] = CreateCommandAllocator(_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		_commandLists[index] = CreateCommandList(
			_device,
			_commandAllocators[index],
			_pipelineState,
			D3D12_COMMAND_LIST_TYPE_DIRECT);
	}

	//_pipelineState = CreatePipelineState(_device);

	_fence = CreateFence(_device);
	_fenceValue = 1;
	_fenceEvent = CreateEventHandle();
}

void Renderer::Uninitialize()
{
	CloseHandle(_fenceEvent);
}

void Renderer::Render()
{
	auto commandAllocator = _commandAllocators[_currentFrameIndex];
	auto result = commandAllocator->Reset();
	ThrowOnFail(result);

	auto commandList = _commandLists[_currentFrameIndex];
	result = commandList->Reset(commandAllocator.Get(), _pipelineState.Get());
	ThrowOnFail(result);

	auto &backBuffer = _backBuffers[_currentFrameIndex];

	auto presentToRtvBarrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		backBuffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	commandList->ResourceBarrier(1, &presentToRtvBarrier);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv
	(
		_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		_currentFrameIndex,
		_rtvDescriptorSize
	);

	float clearColor[] = { .4f, .6f, .9f, 1.0f };
	commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

	auto rtvToPresentBarrier = CD3DX12_RESOURCE_BARRIER::Transition
	(
		backBuffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	commandList->ResourceBarrier(1, &rtvToPresentBarrier);

	result = commandList->Close();
	ThrowOnFail(result);

	ID3D12CommandList* const commandLists[] =
	{
		commandList.Get()
	};
	_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

	auto syncInterval = 1;
	auto presentFlags = 0;

	result = _swapChain->Present(syncInterval, presentFlags);
	ThrowOnFail(result);

	auto fence = ++_fenceValue;
	result = _commandQueue->Signal(_fence.Get(), fence);
	ThrowOnFail(result);

	if (_fence->GetCompletedValue() < fence)
	{
		result = _fence->SetEventOnCompletion(fence, _fenceEvent);
		WaitForSingleObject(_fenceEvent, INFINITE);
	}

	_currentFrameIndex = _swapChain->GetCurrentBackBufferIndex();
}