#include "SampleState.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <tchar.h>
#include <cstdint>
#include <chrono>

void PcGame::Driver::SampleState::Update()
{
	static uint64_t frameCount = 0;
	static double elapsedSeconds = 0.0;
	static std::chrono::high_resolution_clock clock;
	static auto previousTime = clock.now();

	frameCount++;
	auto now = clock.now();
	auto delta = now - previousTime;
	previousTime = now;

	elapsedSeconds += delta.count() * 1e-9;

	if (elapsedSeconds > 1)
	{
		char buffer[500];
		ZeroMemory(buffer, 500);
		auto fps = frameCount / elapsedSeconds;
		if (serviceManager()->get<Engine::ServiceManager>("service-manager") != nullptr)
		{
			sprintf_s(buffer, 500, "Sample State FPS: %f\n", fps);
		}
		else
		{
			sprintf_s(buffer, 500, "NULLPTR Sample State FPS: %f\n", fps);
		}
		wchar_t wbuffer[500];
		MultiByteToWideChar(CP_ACP, 0, buffer, -1, wbuffer, 500);
		OutputDebugString(wbuffer);

		frameCount = 0;
		elapsedSeconds = 0.0;
	}
}
