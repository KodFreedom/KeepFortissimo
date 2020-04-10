//--------------------------------------------------------------------------------
//  defined the structures for render
//  render用情報定義
//  定义render用情报
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <vector>
#include "../system_setting.h"
using namespace DirectX;

namespace KeepFortissimo
{
    //--------------------------------------------------------------------------------
    //  enum
    //--------------------------------------------------------------------------------
    enum class RenderPriority : uint32_t
    {
        kOpaque,
        kMax
    };

    //--------------------------------------------------------------------------------
    //  const variable / 定数 / 定量
    //--------------------------------------------------------------------------------
    static constexpr uint32_t sc_num_frame_resources = 3;

    //--------------------------------------------------------------------------------
    //  struct / 構造体 / 构造体
    //--------------------------------------------------------------------------------
    struct VertexTest
    {
        XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
        XMFLOAT4 color = XMFLOAT4(DirectX::Colors::White);
    };

    struct Vertex3d
    {
        XMFLOAT3 position = { 0.0f, 0.0f, 0.0f };
        XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };
        XMFLOAT3 tangent = { 0.0f, 0.0f, 0.0f };
        XMFLOAT2 uv = { 0.0f, 0.0f };
    };

    struct MeshData
    {
        std::vector<Vertex3d> vertices;
        std::vector<uint32_t> indeces;
    };

    // Test
    struct ObjectConstants
    {
        XMMATRIX world = XMMatrixIdentity();
    };

    struct PassConstants
    {
        XMMATRIX View = XMMatrixIdentity();
        XMMATRIX InvView = XMMatrixIdentity();
        XMMATRIX Proj = XMMatrixIdentity();
        XMMATRIX InvProj = XMMatrixIdentity();
        XMMATRIX ViewProj = XMMatrixIdentity();
        XMMATRIX InvViewProj = XMMatrixIdentity();
        XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
        float cbPerObjectPad1 = 0.0f;
        XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
        XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
        float NearZ = 0.0f;
        float FarZ = 0.0f;
        float TotalTime = 0.0f;
        float DeltaTime = 0.0f;
    };
}