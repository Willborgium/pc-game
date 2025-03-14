#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Renderer.h"
#include "IState.h"
#include "ServiceManager.h"

namespace PcGame::Engine
{
	class Game
	{
	public:
		Game(IState* initialState);
		void Initialize(ServiceManager* serviceManager, LPCWSTR appName, HINSTANCE hInstance, int width, int height, int nCmdShow);
		int Run();
		void Uninitialize();
	private:
		void CreateAppWindow(LPCWSTR appName, HINSTANCE hInstance, int width, int height, int nCmdShow);

		bool _isInitialized;
		bool _isRunning;

		HWND _hWnd;
		Renderer* _renderer;
		IState* _state;
	};
};