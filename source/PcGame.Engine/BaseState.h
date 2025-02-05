#pragma once

#include "IState.h"
#include "ServiceManager.h"

namespace PcGame::Engine
{
	class BaseState : public IState
	{
	public:
		BaseState(ServiceManager* serviceManager)
			: _serviceManager(serviceManager), _isInitialized(false)
		{
		}
		bool IsInitialized() { return _isInitialized; }
		void Initialize() { _isInitialized = true; }
	protected:
		ServiceManager* serviceManager() { return _serviceManager; }
		bool _isInitialized;
	private:
		ServiceManager* _serviceManager;
	};

	typedef IState* (*StateFactory)(ServiceManager*);
};