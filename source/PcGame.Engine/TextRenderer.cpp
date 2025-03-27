#include "TextRenderer.h"

#include <d3dx12.h>
#include <fstream>
#include <sstream>

using namespace PcGame::Engine;

#include <DirectXMath.h>

using namespace DirectX;

// #include <DirectXTex.h>

using namespace DirectX;

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT2 texcoord;
};

TextRenderer::TextRenderer()
{
}

TextRenderer::~TextRenderer()
{
}

ComPtr<ID3D12Resource> LoadTexture(Renderer* renderer, const std::wstring& path)
{
    return nullptr;
 //   // Load the image data using DirectXTex
 //   TexMetadata metadata;
 //   ScratchImage scratchImage;
 //   HRESULT hr = LoadFromWICFile(path.c_str(), WIC_FLAGS_NONE, &metadata, scratchImage);
 //   if (FAILED(hr))
 //   {
 //       throw std::runtime_error("Failed to load texture");
 //   }

 //   // Create the texture resource
 //   D3D12_RESOURCE_DESC textureDesc = {};
 //   textureDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
 //   textureDesc.Format = metadata.format;
 //   textureDesc.Width = static_cast<UINT>(metadata.width);
 //   textureDesc.Height = static_cast<UINT>(metadata.height);
 //   textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
 //   textureDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
 //   textureDesc.SampleDesc.Count = 1;
 //   textureDesc.SampleDesc.Quality = 0;
 //   textureDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);

 //   CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
 //   CD3DX12_RESOURCE_DESC resourceDesc(textureDesc);
 //   auto texture = renderer->CreateCommittedResource(&heapProperties,&resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST);

 //   // Upload the image data to the texture resource
 //   const Image* image = scratchImage.GetImages();
 //   const UINT64 uploadBufferSize = GetRequiredIntermediateSize(texture.Get(), 0, static_cast<UINT>(metadata.mipLevels));

 //   CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
 //   CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

 //   auto textureUploadHeap = renderer->CreateCommittedResource(&uploadHeapProperties, &uploadResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ);

 //   D3D12_SUBRESOURCE_DATA textureData = {};
 //   textureData.pData = image->pixels;
 //   textureData.RowPitch = image->rowPitch;
 //   textureData.SlicePitch = image->slicePitch;

 //   renderer->Update(texture.Get(), textureUploadHeap.Get(), 0, 0, static_cast<UINT>(metadata.mipLevels), &textureData);

	//renderer->ResourceBarrier(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

 //   return texture;
}

void TextRenderer::Initialize(Renderer* renderer, const std::wstring& fontTexturePath, const std::wstring& fontDescriptorPath)
{
    LoadFont(renderer, fontTexturePath, fontDescriptorPath);
}

void TextRenderer::LoadFont(Renderer* renderer, const std::wstring& fontTexturePath, const std::wstring& fontDescriptorPath)
{
    // THIS IS BROKEN
    return;

    // Load the font texture
    _fontTexture = LoadTexture(renderer, fontTexturePath);

    // Create a shader resource view (SRV) for the font texture
    _srvHeap = renderer->CreateDescriptorHeap(1, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = _fontTexture->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
	renderer->CreateShaderResourceView(_fontTexture.Get(), &srvDesc, _srvHeap->GetCPUDescriptorHandleForHeapStart());

    // Load the font descriptor
    std::wifstream file(fontDescriptorPath);
    std::wstring line;
    while (std::getline(file, line))
    {
        std::wistringstream iss(line);
        wchar_t character;
        CharacterInfo info;
        iss >> character >> info.u >> info.v >> info.width >> info.height >> info.xOffset >> info.yOffset >> info.xAdvance;
        _characterInfo[character] = info;
    }
}

void RenderQuad(ComPtr<ID3D12GraphicsCommandList> commandList, const Vertex vertices[4])
{
	// (Assume a function RenderQuad exists that renders a quad with the given vertices)
}

void TextRenderer::RenderText(ComPtr<ID3D12GraphicsCommandList> commandList, const std::wstring& text, float x, float y, float scale)
{
    // Set the SRV heap
    ID3D12DescriptorHeap* heaps[] = { _srvHeap.Get() };
    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    // Render each character
    for (const wchar_t& character : text)
    {
        if (_characterInfo.find(character) == _characterInfo.end())
            continue;

        const CharacterInfo& info = _characterInfo[character];

        // Calculate the position and size of the quad
        float xpos = x + info.xOffset * scale;
        float ypos = y - (info.height - info.yOffset) * scale;
        float w = info.width * scale;
        float h = info.height * scale;

        // Set up the vertices for the quad
        Vertex vertices[] =
        {
            { { xpos,     ypos + h, 0.0f }, { info.u, info.v + info.height } },
            { { xpos,     ypos,     0.0f }, { info.u, info.v } },
            { { xpos + w, ypos + h, 0.0f }, { info.u + info.width, info.v + info.height } },
            { { xpos + w, ypos,     0.0f }, { info.u + info.width, info.v } }
        };

        // Render the quad
        // (Assume a function RenderQuad exists that renders a quad with the given vertices)
        RenderQuad(commandList, vertices);

        // Advance the cursor to the next position
        x += info.xAdvance * scale;
    }
}