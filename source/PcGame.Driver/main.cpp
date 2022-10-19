#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <EntryPoint.h>

#include "SampleState.h"

using namespace PcGame;

#pragma comment(lib, "PcGame.Engine.lib")

Engine::IState* createInitialState(Engine::ServiceManager* serviceManager)
{
	return new Driver::SampleState(serviceManager);
}

Engine::InitializationData Engine::InitializeApp(Engine::ServiceManager* serviceManager)
{
	auto data = Engine::InitializationData();

	data.appName = L"PC Game";
	data.windowWidth = 1920;
	data.windowHeight = 1080;
	data.initialStateFactory = &createInitialState;

	serviceManager->set("service-manager", serviceManager);

	return data;
}