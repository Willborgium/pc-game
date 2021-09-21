#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace PcGame::Engine
{
	struct InitializationData
	{
		LPCWSTR appName;
		int windowWidth;
		int windowHeight;
	};

	extern InitializationData InitializeApp();
};