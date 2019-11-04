//--------------------------------------------------------------------------------
//  upload buffer class
//  アップロードバッファクラス
//  上传缓冲区类
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <wrl.h>
#include <d3d12.h>
#include "../system_setting.h"
#include "../../Utilities/kf_utilities.h"
#include "../../Utilities/exception.h"
#include "../../Libraries/DirectX12/d3dx12.h"
using Microsoft::WRL::ComPtr;

namespace KeepFortissimo
{
    template<typename T>
    class UploadBuffer
    {
    public:
        UploadBuffer(ID3D12Device* device, const u32 element_count, const bool is_constant_buffer)
            : m_is_constant_buffer(is_constant_buffer)
        {
            m_element_byte_size = sizeof(T);

            // 常量缓冲区的大小为256b的整数倍
            // 这是因为硬件只能按m*256b的偏移量和n*256de数据长度这两种规格来查看常量数据
            // Constant buffer elements need to be multiples of 256 bytes.
            // This is because the hardware can only view constant data 
            // at m*256 byte offsets and of n*256 byte lengths. 
            // typedef struct D3D12_CONSTANT_BUFFER_VIEW_DESC {
            // UINT64 OffsetInBytes; // multiple of 256
            // UINT   SizeInBytes;   // multiple of 256
            // } D3D12_CONSTANT_BUFFER_VIEW_DESC;
            if (m_is_constant_buffer)
                m_element_byte_size = Utility::CalculateConstantBufferByteSize(sizeof(T));

            ThrowIfFailed(device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(m_element_byte_size * element_count),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_upload_buffer)));

            ThrowIfFailed(m_upload_buffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mapped_data)));

            // 只要还会修改当前的资源，我们就无需取消映射
            // 但是在资源被gpu使用期间，我们千万不可向该资源进行写操作（所以必须借助于同步技术）
            // We do not need to unmap until we are done with the resource.  However, we must not write to
            // the resource while it is in use by the GPU (so we must use synchronization techniques).
        }

        ~UploadBuffer()
        {
            if (m_upload_buffer != nullptr)
                m_upload_buffer->Unmap(0, nullptr);

            m_mapped_data = nullptr;
        }

        ID3D12Resource* Resource() const
        {
            return m_upload_buffer.Get();
        }

        void CopyData(const int element_index, const T& data)
        {
            memcpy(&m_mapped_data[element_index * m_element_byte_size], &data, sizeof(T));
        }

    private:
        UploadBuffer() = delete;
        UploadBuffer(const UploadBuffer& rhs) = delete;
        UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

        ComPtr<ID3D12Resource> m_upload_buffer = nullptr;
        BYTE*                  m_mapped_data = nullptr;
        u32                    m_element_byte_size = 0;
        bool                   m_is_constant_buffer = false;
    };
}