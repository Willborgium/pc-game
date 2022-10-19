#include "ServiceManager.h"

void PcGame::Engine::ServiceManager::set(const char* key, void* service)
{
	_services[key] = service;
}

void* PcGame::Engine::ServiceManager::get(const char* key)
{
	if (_services.contains(key))
	{
		return _services[key];
	}

	return nullptr;
}

void* PcGame::Engine::ServiceManager::clear(const char* key)
{
	auto value = get(key);

	if (nullptr != value)
	{
		_services[key] = nullptr;
	}

	return value;
}
