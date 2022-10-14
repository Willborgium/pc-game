#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace PcGame::Engine
{
	void ThrowOnFail(HRESULT result);
	void DisplayMessage(LPCWSTR title, LPCWSTR message);
}