#pragma once

#include "Renderer.h"

#include <wrl.h>
using namespace Microsoft::WRL;
#include <d3d12.h>
#include <unordered_map>
#include <string>

namespace PcGame::Engine
{
	struct CharacterInfo
	{
		float u, v, width, height, xOffset, yOffset, xAdvance;
	};

	class TextRenderer
	{
	public:
		TextRenderer();
		~TextRenderer();

		void Initialize(Renderer* renderer, const std::wstring& fontTexturePath, const std::wstring& fontDescriptorPath);
		void RenderText(ComPtr<ID3D12GraphicsCommandList> commandList, const std::wstring& text, float x, float y, float scale);

	private:
		void LoadFont(Renderer* renderer, const std::wstring& fontTexturePath, const std::wstring& fontDescriptorPath);

		ComPtr<ID3D12Resource> _fontTexture;
		ComPtr<ID3D12DescriptorHeap> _srvHeap;
		std::unordered_map<wchar_t, CharacterInfo> _characterInfo;
	};
}

