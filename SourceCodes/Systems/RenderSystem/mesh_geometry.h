//--------------------------------------------------------------------------------
//  mesh geometry define
//  メッシュの定義
//  几何图形结构体
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <DirectXCollision.h>
#include <unordered_map>
#include "../system_setting.h"
using Microsoft::WRL::ComPtr;

namespace KeepFortissimo
{
    // 它提供了对存于顶点缓冲区和索引缓冲区中的单个几何体
    // 进行绘制所需的数据和偏移量，我们可以据此来实现复数几何体存储与一个大缓冲区（6.3节p186）
    // Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
    // geometries are stored in one vertex and index buffer.  It provides the offsets
    // and data needed to draw a subset of geometry stores in the vertex and index 
    // buffers so that we can implement the technique described by Figure 6.3.
    struct UnitMeshGeometry
    {
        u32 index_count = 0;
        u32 start_index_location = 0;
        u32 base_vertex_location = 0;

        // 通过此子网格来定义当前结构体中所存几何体的包围盒。
        // Bounding box of the geometry defined by this submesh. 
        // This is used in later chapters of the book.
        DirectX::BoundingBox bounding_box;
    };

    struct MeshGeometry
    {
        // 指定此几何体网格集合的名称，这样我们就能根据此找到他
        // Give it a name so we can look it up by name.
        std::string name;

        // 系统内存中的副本，由于顶点/索引可以是泛型格式，所以用Blob类型
        // 来表示带用户在使用时再将其转换为适当的类型
        // System memory copies.  Use Blobs because the vertex/index format can be generic.
        // It is up to the client to cast appropriately.  
        ComPtr<ID3DBlob> vertex_buffer_cpu = nullptr;
        ComPtr<ID3DBlob> index_buffer_cpu = nullptr;

        ComPtr<ID3D12Resource> vertex_buffer_gpu = nullptr;
        ComPtr<ID3D12Resource> index_buffer_gpu = nullptr;

        ComPtr<ID3D12Resource> vertex_buffer_uploader = nullptr;
        ComPtr<ID3D12Resource> index_buffer_uploader = nullptr;

        // 与缓冲区相关的数据
        // Data about the buffers.
        u32 vertex_byte_stride = 0;
        u32 vertex_buffer_byte_size = 0;
        DXGI_FORMAT index_format = DXGI_FORMAT_R16_UINT;
        u32 index_buffer_byte_size = 0;

        // 一个结构体能够存储一组顶点/索引缓冲区中的多个几何体
        // 若利用下列容器来定义子网格几何体，我们就能单独地绘制出其中的子网格（单个几何体）
        // A MeshGeometry may store multiple geometries in one vertex/index buffer.
        // Use this container to define the Submesh geometries so we can draw
        // the Submeshes individually.
        std::unordered_map<std::string, UnitMeshGeometry> unit_mesh_geometries;

        D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
        {
            D3D12_VERTEX_BUFFER_VIEW vbv;
            vbv.BufferLocation = vertex_buffer_gpu->GetGPUVirtualAddress();
            vbv.StrideInBytes = vertex_byte_stride;
            vbv.SizeInBytes = vertex_buffer_byte_size;

            return vbv;
        }

        D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
        {
            D3D12_INDEX_BUFFER_VIEW ibv;
            ibv.BufferLocation = index_buffer_gpu->GetGPUVirtualAddress();
            ibv.Format = index_format;
            ibv.SizeInBytes = index_buffer_byte_size;

            return ibv;
        }

        // 待数据上传至GPU后，我们就能释放这些内存了
        // We can free this memory after we finish upload to the GPU.
        void DisposeUploaders()
        {
            vertex_buffer_uploader = nullptr;
            index_buffer_uploader = nullptr;
        }
    };
}