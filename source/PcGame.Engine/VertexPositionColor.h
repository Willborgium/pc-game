#pragma once

#include <DirectXMath.h>

using namespace DirectX;

namespace PcGame::Engine
{
    struct VertexPositionColor
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };
}