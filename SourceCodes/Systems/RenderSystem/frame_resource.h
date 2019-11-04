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
#pragma once
#include "upload_buffer.h"
#include "render_define.h"

namespace KeepFortissimo
{
    class FrameResource
    {
    public:
        //--------------------------------------------------------------------------------
        //  constructor
        //  コンストラクタ
        //  构造函数
        //--------------------------------------------------------------------------------
        FrameResource(ID3D12Device* device, u32 pass_count, u32 object_count);

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  析构函数
        //--------------------------------------------------------------------------------
        ~FrameResource();

        //--------------------------------------------------------------------------------
        //  CurrentFence getter/setter
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  CurrentFence の取得と設定
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  设定与取得现在的围栏
        //--------------------------------------------------------------------------------
        u64  CurrentFence() const { return m_current_fence; }
        void SetCurrentFence(const u64 value) { m_current_fence = value; }

        //--------------------------------------------------------------------------------
        //  ObjectCbuffer getter/setter
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  ObjectCbuffer の取得と設定
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  设定与取得ObjectCbuffer
        //--------------------------------------------------------------------------------
        UploadBuffer<ObjectConstants>* ObjectCbuffer() { return m_object_cbuffer; }

        //--------------------------------------------------------------------------------
        //  PassCbuffer getter/setter
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  PassCbuffer の取得と設定
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  设定与取得PassCbuffer
        //--------------------------------------------------------------------------------
        UploadBuffer<PassConstants>* PassCbuffer() { return m_pass_cbuffer; }

        //--------------------------------------------------------------------------------
        //  CommandListAllocator getter/setter
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  CommandListAllocator の取得と設定
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  设定与取得CommandListAllocator
        //--------------------------------------------------------------------------------
        ComPtr<ID3D12CommandAllocator> CommandListAllocator() { return m_command_list_allocator; }

    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        FrameResource() = delete;
        FrameResource(const FrameResource& rhs) = delete;
        FrameResource& operator=(const FrameResource& rhs) = delete;

        // 在gpu处理完于此命令分配器相关的命令之前，不能对他进行重置
        // 所以每一帧都要有他们自己的命令分配器
        // We cannot reset the allocator until the GPU is done processing the commands.
        // So each frame needs their own allocator.
        ComPtr<ID3D12CommandAllocator> m_command_list_allocator = nullptr;

        // 在gpu执行完引用此常量缓冲区的命令之前,我们不能对他进行更新
        // 因此每一帧都要有他们自己的常量缓冲区
        // We cannot update a cbuffer until the GPU is done processing the commands
        // that reference it.  So each frame needs their own cbuffers.
        UploadBuffer<PassConstants>* m_pass_cbuffer = nullptr;
        UploadBuffer<ObjectConstants>* m_object_cbuffer = nullptr;

        // 通过围栏值将命令标记到此围栏点,这使我们可以检测到gpu是否还在使用这些帧资源
        // Fence value to mark commands up to this fence point.  This lets us
        // check if these frame resources are still in use by the GPU.
        u64 m_current_fence = 0;
    };
}