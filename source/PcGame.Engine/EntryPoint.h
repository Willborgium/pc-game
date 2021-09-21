#pragma once
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