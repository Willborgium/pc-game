#include "Model.h"
#include "Utilities.h"

using namespace PcGame::Engine;

Model::Model(ComPtr<ID3D12Device2> device, std::vector<Mesh> meshes)
	: _meshes(meshes), _position(0, 0, 0), _rotation(0, 0, 0), _scale(1, 1, 1)
{
	_worldBuffer = CreateConstantBuffer(device, sizeof(DirectX::XMMATRIX) * 3);
}

void Model::Draw(ComPtr<ID3D12GraphicsCommandList> commandList)
{
	auto scale = DirectX::XMMatrixScaling(_scale.x, _scale.y, _scale.z);
	auto rotation = DirectX::XMMatrixRotationRollPitchYaw(_rotation.x, _rotation.y, _rotation.z);
	auto translation = DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);

	DirectX::XMMATRIX world[3] = { scale, rotation, translation};

	UpdateConstantBuffer(_worldBuffer, &world, sizeof(world));
	commandList->SetGraphicsRootConstantBufferView(1, _worldBuffer->GetGPUVirtualAddress());

    for (auto& mesh : _meshes)
    {
        mesh.Draw(commandList);
    }
}
