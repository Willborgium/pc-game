#pragma once

#include <BaseState.h>

#include "Model.h"
#include "ICamera.h"
#include "Renderer.h"

using namespace PcGame::Engine;

namespace PcGame::Driver
{
	class SampleState : public Engine::BaseState
	{
	public:
		SampleState(ServiceManager* serviceManager)
			: BaseState(serviceManager), _primitive(nullptr), _camera(nullptr), _renderer(nullptr)
		{
			_serviceManager = serviceManager;
		}

		void Initialize();
		void Update();
		void Render(ComPtr<ID3D12GraphicsCommandList> commandList);

	private:
		Model* _primitive;
		ICamera* _camera;

		Renderer* _renderer;
		ServiceManager* _serviceManager;
	};
}