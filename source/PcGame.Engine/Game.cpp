#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <tchar.h>

#include "Game.h"
#include "EntryPoint.h"
#include "WndProc.h"
#include "Utilities.h"

using namespace PcGame::Engine;

#include <chrono>

Game::Game()
{
	_isInitialized = false;
	_isRunning = false;
	_hWnd = nullptr;
	_renderer = nullptr;
}

void Game::Initialize(LPCWSTR appName, HINSTANCE hInstance, int width, int height, int nCmdShow)
{
	CreateAppWindow(appName, hInstance, width, height, nCmdShow);

	_renderer = new Renderer();

	_renderer->Initialize(_hWnd, width, height);

	_isInitialized = true;
}

void Game::CreateAppWindow(LPCWSTR appName, HINSTANCE hInstance, int width, int height, int nCmdShow)
{
	WNDCLASSEX setup;
	setup.cbSize = sizeof(WNDCLASSEX);
	setup.style = CS_HREDRAW | CS_VREDRAW;
	setup.lpfnWndProc = PcGame::WndProc;
	setup.cbClsExtra = 0;
	setup.cbWndExtra = 0;
	setup.hInstance = hInstance;
	setup.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	setup.hCursor = LoadCursor(nullptr, IDC_ARROW);
	setup.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	setup.lpszMenuName = nullptr;
	setup.lpszClassName = appName;
	setup.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	auto result = RegisterClassEx(&setup);
	if (!result)
	{
		DisplayMessage(_T("Call failed"), _T("Call to RegisterClassEx failed!"));
		return;
	}

	_hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		appName,
		appName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!_hWnd)
	{
		DisplayMessage(_T("Call failed"), _T("Call to CreateWindowEx failed!"));
		return;
	}

	ShowWindow(_hWnd, nCmdShow);
	UpdateWindow(_hWnd);
}

void Update()
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
		sprintf_s(buffer, 500, "FPS: %f\n", fps);
		wchar_t wbuffer[500];
		MultiByteToWideChar(CP_ACP, 0, buffer, -1, wbuffer, 500);
		OutputDebugString(wbuffer);

		frameCount = 0;
		elapsedSeconds = 0.0;
	}
}

int Game::Run()
{
	if (!_isInitialized)
	{
		return 0;
	}

	_isRunning = true;

	MSG message{};

	while (message.message != WM_QUIT)
	{
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			Update();
			_renderer->Render();
		}
	}

	_isRunning = false;

	return (int)message.wParam;
}

void Game::Uninitialize()
{
	_renderer->Uninitialize();
	delete _renderer;
}