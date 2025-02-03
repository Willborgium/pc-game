#include "KeyboardInputHandler.h"

using namespace PcGame::Engine;

KeyboardInputHandler* KeyboardInputHandler::_instance = nullptr;

KeyboardInputHandler::KeyboardInputHandler()
{
	if (_instance == nullptr)
	{
		_instance = this;
	}
	else
	{
		throw std::exception("KeyboardInputHandler instance already exists");
	}
}

void KeyboardInputHandler::HandleKeyEvent(char key, bool isDown)
{
	auto keyEnum = static_cast<Keys>(key);

	if (isDown)
	{
		_downKeys.insert(keyEnum);
	}
	else
	{
		_downKeys.erase(keyEnum);
	}
}