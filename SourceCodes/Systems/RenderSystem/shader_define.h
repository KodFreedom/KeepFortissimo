//--------------------------------------------------------------------------------
//  defined the structures for shader
//  shader用情報定義
//  定义shader用情报
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <DirectXMath.h>
using namespace DirectX;

namespace KeepFortissimo
{
    struct Vertex3d
    {
        XMFLOAT3 position;
        XMFLOAT4 color;
    };

    struct ObjectConstants
    {
        XMMATRIX world_view_projection = XMMatrixIdentity();
    };
}