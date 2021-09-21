#include "Utilities.h"

#include <exception>

void PcGame::Engine::ThrowOnFail(HRESULT result)
{
	if (FAILED(result))
	{
		throw std::exception();
	}
}
