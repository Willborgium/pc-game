#pragma once

namespace PcGame::Engine
{
	class IState
	{
	public:
		virtual void Update() = 0;
	};
};