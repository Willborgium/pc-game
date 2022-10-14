#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Game.h"
#include "EntryPoint.h"

int WINAPI WinMain(
	_In_ HINSTANCE		hInstance,
	_In_opt_ HINSTANCE	hPrevInstance,
	_In_ LPSTR			lpCmdLine,
	_In_ int			nCmdShow
)
{
	const auto data = PcGame::Engine::InitializeApp();

	PcGame::Engine::Game game;

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