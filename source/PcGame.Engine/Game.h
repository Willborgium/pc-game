#pragma once
#include <Windows.h>

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
		bool _isInitialized;
		bool _isRunning;

		HWND _hWnd;
	};
};