#pragma once

#include "IState.h"
#include "ServiceManager.h"

namespace PcGame::Engine
{
	class BaseState : public IState
	{
	public:
		BaseState(ServiceManager* serviceManager)
			: _serviceManager(serviceManager)
		{
		}
	protected:
		ServiceManager* serviceManager() { return _serviceManager; }
	private:
		ServiceManager* _serviceManager;
	};

	typedef IState* (*StateFactory)(ServiceManager*);
};