#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Game.h"
#include "EntryPoint.h"

#include "KeyboardInputHandler.h"

using namespace PcGame::Engine;

int WINAPI WinMain(
	_In_ HINSTANCE		hInstance,
	_In_opt_ HINSTANCE	hPrevInstance,
	_In_ LPSTR			lpCmdLine,
	_In_ int			nCmdShow
)
{
	new KeyboardInputHandler();

	auto serviceManager = new ServiceManager();

	const auto data = InitializeApp(serviceManager);

	auto initialState = data.initialStateFactory(serviceManager);

	PcGame::Engine::Game game(initialState);

	game.Initialize(
		data.appName,
		hInstance,
		data.windowWidth,
		data.windowHeight,
		nCmdShow);

	auto result = game.Run();

	game.Uninitialize();

	delete initialState;

	return result;
}