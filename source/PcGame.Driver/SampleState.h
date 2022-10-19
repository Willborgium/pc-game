#pragma once

#include <BaseState.h>

namespace PcGame::Driver
{
	class SampleState : public Engine::BaseState
	{
	public:
		SampleState(Engine::ServiceManager* serviceManager)
			: Engine::BaseState(serviceManager)
		{
		}

		void Update();
	};
}