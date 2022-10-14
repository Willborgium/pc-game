#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace PcGame
{
	LRESULT CALLBACK WndProc(
		_In_ HWND	hWnd,
		_In_ UINT	message,
		_In_ WPARAM	wParam,
		_In_ LPARAM	lParam);
}