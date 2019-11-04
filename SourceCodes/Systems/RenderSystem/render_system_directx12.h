//--------------------------------------------------------------------------------
//  render system using directx12
//  Directx12描画システム
//  Directx12渲染系统
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include "render_system.h"
using Microsoft::WRL::ComPtr;

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

// Test
#include "upload_buffer.h"
#include "render_define.h"
#include "mesh_geometry.h"

namespace KeepFortissimo
{
    // Lightweight structure stores parameters to draw a shape.  This will
    // vary from app-to-app.
    struct RenderItem
    {
        // World matrix of the shape that describes the object's local space
        // relative to the world space, which defines the position, orientation,
        // and scale of the object in the world.
        XMMATRIX World = XMMatrixIdentity();

        // 用已更新标志(dirty flag)来表示物体的相关数据已发生改变
        // 这意味着我们此时需要更新常量缓冲区,由于每个frameresource中都有一个物体常量缓冲区
        // 所以我们必须对每个frameresource都进行更新
        // Dirty flag indicating the object data has changed and we need to update the constant buffer.
        // Because we have an object cbuffer for each FrameResource, we have to apply the
        // update to each FrameResource.  Thus, when we modify obect data we should set 
        // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
        int NumFramesDirty = sc_num_frame_resources;

        // 该索引指向的gpu常量缓冲区对应于当前渲染项中的物体常量缓冲区
        // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
        u32 ObjCBIndex = -1;

        // 此渲染项参与绘制的几何体.注意绘制一个集合体可能会用到多个渲染项
        MeshGeometry* Geo = nullptr;

        // Primitive topology.
        D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        // DrawIndexedInstanced parameters.
        UINT IndexCount = 0;
        UINT StartIndexLocation = 0;
        int BaseVertexLocation = 0;
    };

    //--------------------------------------------------------------------------------
    // 前方宣言
    //--------------------------------------------------------------------------------
    class FrameResource;

    class RenderSystemDirectX12 : public RenderSystem
    {
        friend class RenderSystem;

    public:
        //--------------------------------------------------------------------------------
        //  Render all registered components
        //  登録したコンポネントを描画する
        //  将注册过的组件进行渲染
        //--------------------------------------------------------------------------------
        void Render() override;

        //--------------------------------------------------------------------------------
        //  Called when changed window size
        //  ウインドウサイズ変わった時に呼ばれる
        //  窗口尺寸改变时被呼出
        //--------------------------------------------------------------------------------
        void OnResize() override;

    private:
        //--------------------------------------------------------------------------------
        //  constructor
        //  コンストラクタ
        //  构造函数
        //--------------------------------------------------------------------------------
        RenderSystemDirectX12();

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  析构函数
        //--------------------------------------------------------------------------------
        ~RenderSystemDirectX12();

        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        RenderSystemDirectX12(RenderSystemDirectX12 const&) = delete;
        void operator=(RenderSystemDirectX12 const&) = delete;

        //--------------------------------------------------------------------------------
        //  initialize the instance
        //  初期化処理
        //  初始化
        //--------------------------------------------------------------------------------
        bool Initialize() override;

        //--------------------------------------------------------------------------------
        //  Uninit the instance
        //  インスタンスの終了処理
        //  终了处理
        //--------------------------------------------------------------------------------
        void Uninitialize() override;

        //--------------------------------------------------------------------------------
        //  Create the directx12 device
        //  デバイスの生成
        //  生成DirectX12的Device
        //--------------------------------------------------------------------------------
        void CreateDevice();

        //--------------------------------------------------------------------------------
        //  Save the descriptor sizes
        //  descriptor sizeの保存
        //  保存descriptor size
        //--------------------------------------------------------------------------------
        void SaveDescriptorSize();

        //--------------------------------------------------------------------------------
        //  Check MSAA quality support
        //  MSAA quality supportの確認
        //  确认是否支持抗锯齿及其质量
        //--------------------------------------------------------------------------------
        void CheckMsaaQuality();

        //--------------------------------------------------------------------------------
        //  Create the descriptor heap of render target view and depth stencil view
        //  RtvとDsvのdescriptor heapを作成
        //  生成Rtv和Dsv的descriptor heap
        //--------------------------------------------------------------------------------
        void CreateRtvAndDsvDescriptorHeaps();

        //--------------------------------------------------------------------------------
        //  Create the command queue, command allocator and command list
        //  command queue, command allocator, command listの作成
        //  生成command queue, command allocator, command list
        //--------------------------------------------------------------------------------
        void CreateCommandObjects();

        //--------------------------------------------------------------------------------
        //  Create the swap chain
        //  swap chainの生成
        //  生成swap chain
        //--------------------------------------------------------------------------------
        void CreateSwapChain();

        //--------------------------------------------------------------------------------
        //  Upload to command queue
        //  command queueにアップロードする
        //  上传命令至command queue
        //--------------------------------------------------------------------------------
        void FlushCommandQueue();

        //--------------------------------------------------------------------------------
        //  Create the render target view.
        //  render target viewの作成
        //  为swapchain的每一个buffer创建RTV
        //--------------------------------------------------------------------------------
        void CreateRenderTargetView();

        //--------------------------------------------------------------------------------
        //  Create the depth/stencil buffer and view.
        //  depth/stencil buffer and viewの作成
        //  创建深度/模板缓冲区及视图
        //--------------------------------------------------------------------------------
        void CreateDepthStencilBufferView(const UINT width, const UINT height);

        //--------------------------------------------------------------------------------
        //  Create the frame resources
        //  frame resourcesの作成
        //  创建帧资源
        //--------------------------------------------------------------------------------
        void CreateFrameResources();

        //--------------------------------------------------------------------------------
        //  Get the current backbuffer
        //  今のバックバッファを取得する
        //  返回现在的备用缓冲区
        //--------------------------------------------------------------------------------
        ID3D12Resource* CurrentBackBuffer() const;

        //--------------------------------------------------------------------------------
        //  Get the current backbuffer view
        //  今のバックバッファビューを取得する
        //  返回现在的备用缓冲视图
        //--------------------------------------------------------------------------------
        D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;

        //--------------------------------------------------------------------------------
        //  Get the depth stencil view
        //  深度情報ビューを取得する
        //  返回深度缓存视图
        //--------------------------------------------------------------------------------
        D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;

        //--------------------------------------------------------------------------------
        //  update FrameResource
        //  FrameResource更新
        //  更新帧资源
        //--------------------------------------------------------------------------------
        void UpdateFrameResource();

        //--------------------------------------------------------------------------------
        //  const variable / 定数 / 定量
        //--------------------------------------------------------------------------------
        static constexpr u32 sc_swap_chain_buffer_count = 2;

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        ComPtr<IDXGIFactory4>  m_factory = nullptr;
        ComPtr<IDXGISwapChain> m_swap_chain = nullptr;
        ComPtr<ID3D12Device>   m_device = nullptr;

        ComPtr<ID3D12Fence> m_fence = nullptr; // cpu/gpu同步用
        u64 m_current_fence = 0;

        ComPtr<ID3D12CommandQueue>        m_command_queue = nullptr;
        ComPtr<ID3D12CommandAllocator>    m_command_list_allocator = nullptr;
        ComPtr<ID3D12GraphicsCommandList> m_command_list = nullptr;

        u32 m_current_back_buffer = 0;
        ComPtr<ID3D12Resource> m_swap_chain_buffer[sc_swap_chain_buffer_count] = { nullptr };
        ComPtr<ID3D12Resource> m_depth_stencil_buffer = nullptr;

        ComPtr<ID3D12DescriptorHeap> m_rtv_heap = nullptr;
        ComPtr<ID3D12DescriptorHeap> m_dsv_heap = nullptr;

        D3D12_VIEWPORT m_screen_viewport = {};
        D3D12_RECT     m_scissor_rect = {};

        u32 m_rtv_descriptor_size = 0;
        u32 m_dsv_descriptor_size = 0;
        u32 m_cbv_srv_uav_descriptor_size = 0;

        //D3D_DRIVER_TYPE   m_driver_type = D3D_DRIVER_TYPE_HARDWARE;
        DXGI_FORMAT       m_back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT       m_depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        D3D_FEATURE_LEVEL m_feature_level = D3D_FEATURE_LEVEL_12_1;

        FrameResource* m_frame_resources[sc_num_frame_resources] = { nullptr };
        u32            m_current_frame_resource_index = 0;

        // Test
        void InitTest();
        void BuildDescriptorHeaps();
        void BuildConstantBufferViews();
        void BuildRootSignature();
        void BuildGeometry();
        void BuildPSOs();
        void BuildRenderItems();

        void UninitTest();

        void UpdateTest();
        void UpdateCamera();
        void UpdateObjectCBs();
        void UpdateMainPassCB();

        void DrawTest();
        void DrawRenderItems(const std::vector<RenderItem*>& ritems);

        ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* init_data, u64 byte_size, ComPtr<ID3D12Resource>& upload_buffer);
        ComPtr<ID3DBlob> CompileShader(const LPCWSTR filename, const D3D_SHADER_MACRO* defines, const LPCSTR entrypoint, const LPCSTR target);
        
        // 在执行绘制命令前，那些应用程序将绑定到渲染流水线上的资源，
        // 他们会被映射到着色器的对应输入寄存器
        // 根签名一定要于是用他的着色器相兼容
        // 即在绘制开始之前，根签名一定要为着色器提供其执行期间需要绑定到渲染流水线的所有资源
        // 在创建流水线状态对象（pipeline state object）是会对此进行验证
        // 不同的绘制调用可能会用到一组不同的着色器程序，这也就意味这要用到不同的根签名
        ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
        ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

        std::unordered_map<std::string, MeshGeometry*> mGeometries;
        std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
        std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

        ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

        // List of all the render items.
        std::vector<RenderItem*> mAllRitems;

        // Render items divided by PSO.
        std::vector<RenderItem*> mOpaqueRitems;

        PassConstants mMainPassCB;

        UINT mPassCbvOffset = 0;

        XMFLOAT3 mEyePos;
        XMMATRIX mView;
        XMMATRIX mProj;
    };
}