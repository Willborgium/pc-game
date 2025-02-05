#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <vector>
#include <DirectXMath.h>

#include "Mesh.h"
#include "Renderer.h"

using namespace Microsoft::WRL;

namespace PcGame::Engine
{
    class Model
    {
    public:
        Model(Renderer* renderer, std::vector<Mesh> meshes);
        void Draw(ComPtr<ID3D12GraphicsCommandList> commandList);

        DirectX::XMFLOAT3 GetScale() const { return _scale; }
        DirectX::XMFLOAT3 SetScale(DirectX::XMFLOAT3 scale) { return _scale = scale; }
        DirectX::XMFLOAT3 SetScale(float x, float y, float z) { return _scale = DirectX::XMFLOAT3(x, y, z); }
        DirectX::XMFLOAT3 Scale(float x, float y, float z) { return _scale = DirectX::XMFLOAT3(_scale.x + x, _scale.y + y, _scale.z + z); }

        DirectX::XMFLOAT3 GetRotation() const { return _rotation; }
        DirectX::XMFLOAT3 SetRotation(DirectX::XMFLOAT3 rotation) { return _rotation = rotation; }
        DirectX::XMFLOAT3 SetRotation(float x, float y, float z) { return _rotation = DirectX::XMFLOAT3(x, y, z); }
        DirectX::XMFLOAT3 Rotate(float x, float y, float z) { return _rotation = DirectX::XMFLOAT3(_rotation.x + x, _rotation.y + y, _rotation.z + z); }

		DirectX::XMFLOAT3 GetPosition() const { return _position; }
        DirectX::XMFLOAT3 SetPosition(DirectX::XMFLOAT3 position) { return _position = position; }
        DirectX::XMFLOAT3 SetPosition(float x, float y, float z) { return _position = DirectX::XMFLOAT3(x, y, z); }
        DirectX::XMFLOAT3 Translate(float x, float y, float z) { return _position = DirectX::XMFLOAT3(_position.x + x, _position.y + y, _position.z + z); }

    private:
        std::vector<Mesh> _meshes;

        DirectX::XMFLOAT3 _scale;
        DirectX::XMFLOAT3 _rotation;
        DirectX::XMFLOAT3 _position;

        ComPtr<ID3D12Resource> _worldBuffer;
    };
}