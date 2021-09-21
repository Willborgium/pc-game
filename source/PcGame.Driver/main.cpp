#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <EntryPoint.h>
using namespace PcGame;

#pragma comment(lib, "PcGame.Engine.lib")

Engine::InitializationData Engine::InitializeApp()
{
	Engine::InitializationData data;

	data.appName = L"PC Game";
	data.windowWidth = 1920;
	data.windowHeight = 1080;

	return data;
}