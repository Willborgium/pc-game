#include "Utilities.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <tchar.h>

#include <exception>

void PcGame::Engine::ThrowOnFail(HRESULT result)
{
	if (FAILED(result))
	{
		throw std::exception();
	}
}

void PcGame::Engine::DisplayMessage(LPCWSTR title, LPCWSTR message)
{
	MessageBox(nullptr, title, message, 0);
}