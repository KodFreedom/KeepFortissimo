//--------------------------------------------------------------------------------
//  frame resource class
//  フレームリソースクラス
//  帧资源
//  存有CPU为构建每帧命令列表所需的资源
//  其中的数据将依程序而异
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "frame_resource.h"
using namespace KeepFortissimo;

FrameResource::FrameResource(ID3D12Device* device, uint32_t pass_count, uint32_t object_count)
{
    ThrowIfFailed(device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(m_command_list_allocator.GetAddressOf())));

    m_pass_cbuffer = MY_NEW UploadBuffer<PassConstants>(device, pass_count, true);
    m_object_cbuffer = MY_NEW UploadBuffer<ObjectConstants>(device, object_count, true);
}

FrameResource::~FrameResource()
{
    SAFE_DELETE(m_pass_cbuffer);
    SAFE_DELETE(m_object_cbuffer);
}