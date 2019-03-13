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
    public:
        //--------------------------------------------------------------------------------
        //  Create the instance and initialize it
        //  Return：true when succeeded, else false
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  インスタンスの生成と初期化処理
        //  戻り値：成功したらtrue、失敗したらfalse
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  生成实体并初始化
        //  返回值：成功则返回true、反之返回false
        //--------------------------------------------------------------------------------
        static bool StartUp();

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
        static constexpr int kSwapChainBufferCount = 2;

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        Microsoft::WRL::ComPtr<IDXGIFactory4>  factory_;
        Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
        Microsoft::WRL::ComPtr<ID3D12Device>   device_;

        Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
        UINT64 current_fence_;

        Microsoft::WRL::ComPtr<ID3D12CommandQueue>        command_queue_;
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator>    command_list_allocator_;
        Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list_;

        int current_back_buffer_;
        Microsoft::WRL::ComPtr<ID3D12Resource> swap_chain_buffer_[kSwapChainBufferCount];
        Microsoft::WRL::ComPtr<ID3D12Resource> depth_stencil_buffer_;

        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtv_heap_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsv_heap_;

        D3D12_VIEWPORT screen_viewport_;
        D3D12_RECT     scissor_rect_;

        UINT rtv_descriptor_size_;
        UINT dsv_descriptor_size_;
        UINT cbv_srv_uav_descriptor_size_;

        D3D_DRIVER_TYPE   driver_type_;
        DXGI_FORMAT       back_buffer_format_;
        DXGI_FORMAT       depth_stencil_format_;
        D3D_FEATURE_LEVEL feature_level_;
    };
}