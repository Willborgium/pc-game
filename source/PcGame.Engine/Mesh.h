#pragma once

#include <wrl.h>
using namespace Microsoft::WRL;

#include <d3dx12.h>
#include "VertexPositionColor.h"

#include "Renderer.h"

namespace PcGame::Engine
{
    class Mesh
    {
    public:
        Mesh(Renderer* renderer, const std::vector<VertexPositionColor>& vertices, const std::vector<uint32_t>& indices);
        void Draw(ComPtr<ID3D12GraphicsCommandList> commandList) const;

    private:
        ComPtr<ID3D12Resource> _vertexBuffer;
        ComPtr<ID3D12Resource> _indexBuffer;
        D3D12_VERTEX_BUFFER_VIEW _vertexBufferView;
        D3D12_INDEX_BUFFER_VIEW _indexBufferView;
        uint32_t _indexCount;
    };
};