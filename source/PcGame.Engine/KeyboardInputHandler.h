#pragma once

#include <unordered_set>

namespace PcGame::Engine
{
	enum Keys
	{
		A = 'A',
		B = 'B',
		C = 'C',
		D = 'D',
		E = 'E',
		F = 'F',
		G = 'G',
		H = 'H',
		I = 'I',
		J = 'J',
		K = 'K',
		L = 'L',
		M = 'M',
		N = 'N',
		O = 'O',
		P = 'P',
		Q = 'Q',
		R = 'R',
		S = 'S',
		T = 'T',
		U = 'U',
		V = 'V',
		W = 'W',
		X = 'X',
		Y = 'Y',
		Z = 'Z',
		LeftShift = 16,
		LeftControl = 17,
		Spacebar = 32,
		LeftArrow = 37,
		UpArrow = 38,
		RightArrow = 39,
		DownArrow = 40
	};

	class KeyboardInputHandler
	{
	public:
		// Delete copy constructor and assignment operator to prevent copies
		KeyboardInputHandler(const KeyboardInputHandler&) = delete;
		KeyboardInputHandler& operator=(const KeyboardInputHandler&) = delete;

		KeyboardInputHandler();

		bool IsKeyDown(Keys key) const { return _downKeys.find(key) != _downKeys.end(); }

		void HandleKeyEvent(char key, bool isDown);

		static KeyboardInputHandler* GetInstance() { return _instance; }

	private:
		std::unordered_set<Keys> _downKeys;

		static KeyboardInputHandler* _instance;
	};
}
