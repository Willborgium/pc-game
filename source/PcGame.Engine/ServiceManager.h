#pragma once

#include <map>

namespace PcGame::Engine
{
	class ServiceManager
	{
	public:
		template<typename TService>
		TService* get(const char* key)
		{
			auto value = get(key);
			return static_cast<TService*>(value);
		}

		template<typename TService>
		TService* clear(const char* key)
		{
			auto value = clear(key);
			return static_cast<TService*>(value);
		}

		void set(const char* key, void* service);

		void* clear(const char* key);

	private:
		void* get(const char* key);

		std::map<const char*, void*> _services;
	};
};