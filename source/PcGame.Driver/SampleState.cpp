#include "SampleState.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <tchar.h>
#include <cstdint>
#include <chrono>

#include "Utilities.h"
#include "TargetCamera.h"
#include "FreeCamera.h"

using namespace PcGame::Engine;

Mesh CreateTestMesh(Renderer* renderer)
{
	// Initialize the vertices
	std::vector<VertexPositionColor> vertices = {
		// Front face (z = +0.5f)
		{ { -0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f, 1.0f } }, // Vertex 0 - Red
		{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // Vertex 1 - Green
		{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } }, // Vertex 2 - Blue
		{ { -0.5f,  0.5f,  0.5f }, { 1.0f, 1.0f, 0.0f, 1.0f } }, // Vertex 3 - Yellow

		// Back face (z = -0.5f)
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } }, // Vertex 4 - Magenta
		{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } }, // Vertex 5 - Cyan
		{ {  0.5f,  0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // Vertex 6 - White
		{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 0.0f, 1.0f } }, // Vertex 7 - Black
	};

	// Initialize the indices
	std::vector<uint32_t> indices = {
		// Front face
		0, 1, 2,
		0, 2, 3,

		// Back face
		4, 6, 5,
		4, 7, 6,

		// Left face
		4, 5, 1,
		4, 1, 0,

		// Right face
		3, 2, 6,
		3, 6, 7,

		// Top face
		1, 5, 6,
		1, 6, 2,

		// Bottom face
		4, 0, 3,
		4, 3, 7,
	};

	return Mesh(renderer, vertices, indices);
}

void PcGame::Driver::SampleState::Initialize()
{
	_renderer = _serviceManager->get<Renderer>("renderer");

	auto aspectRatio = 1920.0f / 1080.0f;
	float fovAngleY = 60.0f * XM_PI / 180.0f;

	auto targetCamera = new TargetCamera(_renderer, fovAngleY, aspectRatio, 0.01f, 1000.0f);
	targetCamera->SetPosition(0, 0, -2);
	targetCamera->SetTarget(0, 0, 0);

	auto freeCamera = new FreeCamera(_renderer, fovAngleY, aspectRatio, 0.01f, 1000.0f);
	freeCamera->SetPosition(0, 0, -2);

	_camera = targetCamera;

	auto mesh = CreateTestMesh(_renderer);
	_primitive = new Model(_renderer, { mesh });

	BaseState::Initialize();
}

void PcGame::Driver::SampleState::Update()
{
	static uint64_t frameCount = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto previousTime = clock.now();

	frameCount++;
	auto now = clock.now();
	auto delta = now - previousTime;
	previousTime = now;

	elapsedSeconds += delta.count() * 1e-9;

	if (elapsedSeconds > 1)
	{
		char buffer[500];
		ZeroMemory(buffer, 500);
		auto fps = frameCount / elapsedSeconds;
		if (serviceManager()->get<Engine::ServiceManager>("service-manager") != nullptr)
		{
			sprintf_s(buffer, 500, "Sample State FPS: %f\n", fps);
		}
		else
		{
			sprintf_s(buffer, 500, "NULLPTR Sample State FPS: %f\n", fps);
		}
		wchar_t wbuffer[500];
		MultiByteToWideChar(CP_ACP, 0, buffer, -1, wbuffer, 500);
		OutputDebugString(wbuffer);

		frameCount = 0;
		elapsedSeconds = 0.0;
	}

	MoveCamera(_camera);
}

void PcGame::Driver::SampleState::Render(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	_camera->Render(commandList);

	_primitive->Draw(commandList);
}
