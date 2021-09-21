#include <tchar.h>

#include "Game.h"
#include "EntryPoint.h"

using namespace PcGame::Engine;

int WINAPI WinMain(
	_In_ HINSTANCE		hInstance,
	_In_opt_ HINSTANCE	hPrevInstance,
	_In_ LPSTR			lpCmdLine,
	_In_ int			nCmdShow
)
{
	const auto data = InitializeApp();

	Game game;

	game.Initialize(
		data.appName,
		hInstance,
		data.windowWidth,
		data.windowHeight,
		nCmdShow);

	auto result = game.Run();

	game.Uninitialize();

	return result;
}

LRESULT CALLBACK WndProc(
	_In_ HWND	hWnd,
	_In_ UINT	message,
	_In_ WPARAM	wParam,
	_In_ LPARAM	lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

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

	_renderer->DoItAll();

	_isInitialized = true;
}

void Game::CreateAppWindow(LPCWSTR appName, HINSTANCE hInstance, int width, int height, int nCmdShow)
{
	WNDCLASSEX setup;
	setup.cbSize = sizeof(WNDCLASSEX);
	setup.style = CS_HREDRAW | CS_VREDRAW;
	setup.lpfnWndProc = WndProc;
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
		MessageBox(
			nullptr,
			_T("Call to RegisterClassEx failed!"),
			_T("Call failed"),
			0);
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
		MessageBox(
			nullptr,
			_T("Call to CreateWindowEx failed!"),
			_T("Call failed"),
			0);
		return;
	}

	ShowWindow(_hWnd, nCmdShow);
	UpdateWindow(_hWnd);
}

int Game::Run()
{
	if (!_isInitialized)
	{
		return 0;
	}

	_isRunning = true;

	MSG message;

	while (GetMessage(&message, nullptr, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	_isRunning = false;

	return (int)message.wParam;
}

void Game::Uninitialize()
{
}