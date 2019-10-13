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

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

namespace KeepFortissimo
{
    class RenderSystemDirectX12 : public RenderSystem
    {
        friend class RenderSystem;

    public:
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
        //  Prepare for render components
        //  描画する前の準備
        //  渲染前的准备工作
        //--------------------------------------------------------------------------------
        bool PrepareRender() override;

        //--------------------------------------------------------------------------------
        //  End render and present the buffers 
        //  描画終了、バッファの切り替え
        //  渲染完成并交换画面缓存
        //--------------------------------------------------------------------------------
        void EndRender() override;

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
        //  const variable / 定数 / 定量
        //--------------------------------------------------------------------------------
        static constexpr u32 sc_swap_chain_buffer_count = 2;

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        Microsoft::WRL::ComPtr<IDXGIFactory4>  m_factory;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain;
        Microsoft::WRL::ComPtr<ID3D12Device>   m_device;

        Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
        u64 m_current_fence = 0;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue>        m_command_queue;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    m_command_list_allocator;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command_list;

        u32 m_current_back_buffer = 0;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_swap_chain_buffer[sc_swap_chain_buffer_count];
        Microsoft::WRL::ComPtr<ID3D12Resource> m_depth_stencil_buffer;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtv_heap;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsv_heap;

        D3D12_VIEWPORT m_screen_viewport = {};
        D3D12_RECT     m_scissor_rect = {};

        u32 m_rtv_descriptor_size = 0;
        u32 m_dsv_descriptor_size = 0;
        u32 m_cbv_srv_uav_descriptor_size = 0;

        D3D_DRIVER_TYPE   m_driver_type = D3D_DRIVER_TYPE_HARDWARE;
        DXGI_FORMAT       m_back_buffer_format = DXGI_FORMAT_R8G8B8A8_UNORM;
        DXGI_FORMAT       m_depth_stencil_format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        D3D_FEATURE_LEVEL m_feature_level = D3D_FEATURE_LEVEL_12_1;
    };
}