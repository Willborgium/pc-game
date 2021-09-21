#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Renderer.h"

namespace PcGame::Engine
{
	class Game
	{
	public:
		Game();
		void Initialize(LPCWSTR appName, HINSTANCE hInstance, int width, int height, int nCmdShow);
		int Run();
		void Uninitialize();
	private:
		void CreateAppWindow(LPCWSTR appName, HINSTANCE hInstance, int width, int height, int nCmdShow);

		bool _isInitialized;
		bool _isRunning;

		HWND _hWnd;
		Renderer* _renderer;
	};
};