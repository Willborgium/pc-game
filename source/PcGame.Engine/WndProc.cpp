#include "WndProc.h"

#include "KeyboardInputHandler.h"

using namespace PcGame::Engine;

LRESULT CALLBACK PcGame::WndProc(
	_In_ HWND	hWnd,
	_In_ UINT	message,
	_In_ WPARAM	wParam,
	_In_ LPARAM	lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		KeyboardInputHandler::GetInstance()->HandleKeyEvent(static_cast<char>(wParam), true);
		break;

	case WM_KEYUP:
		KeyboardInputHandler::GetInstance()->HandleKeyEvent(static_cast<char>(wParam), false);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}