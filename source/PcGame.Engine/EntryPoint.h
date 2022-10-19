#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "BaseState.h"

#include "ServiceManager.h"

namespace PcGame::Engine
{
	struct InitializationData
	{
		LPCWSTR appName;
		int windowWidth;
		int windowHeight;
		PcGame::Engine::StateFactory initialStateFactory;
	};

	extern InitializationData InitializeApp(ServiceManager* serviceManager);
};