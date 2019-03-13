//--------------------------------------------------------------------------------
//  base class of render system
//  描画システムのベース
//  渲染系统的基类
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "render_system_directx12.h"
#include "main_system.h"
#include "../Utilities/kf_utilities.h"
#include "../Libraries/DirectX12/d3dx12.h"
using namespace KeepFortissimo;
using namespace DirectX;
using Microsoft::WRL::ComPtr;
using namespace std;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  StartUp
//--------------------------------------------------------------------------------
bool RenderSystemDirectX12::StartUp()
{
    if (instance_ != nullptr) return true;
    auto instance = MY_NEW RenderSystemDirectX12();
    return instance->Initialize();
}

//--------------------------------------------------------------------------------
//  OnResize
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::OnResize()
{
    assert(device_);
    assert(swap_chain_);
    assert(command_list_allocator_);

    UINT width = MainSystem::Instance().Width();
    UINT height = MainSystem::Instance().Height();

    // Flush before changing any resources.
    FlushCommandQueue();

    ThrowIfFailed(command_list_->Reset(command_list_allocator_.Get(), nullptr));

    // Release the previous resources we will be recreating.
    for (int i = 0; i < kSwapChainBufferCount; ++i)
    {
        swap_chain_buffer_[i].Reset();
    }

    depth_stencil_buffer_.Reset();

    // Resize the swap chain.
    ThrowIfFailed
    (
        swap_chain_->ResizeBuffers
        (
            kSwapChainBufferCount,
            width,
            height,
            back_buffer_format_,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
        )
    );

    current_back_buffer_ = 0;

    // 为swapchain的每一个buffer创建RTV
    CreateRenderTargetView();

    // Create the depth/stencil buffer and view.
    // 创建深度/模板缓冲区及视图
    CreateDepthStencilBufferView(width, height);

    // Execute the resize commands.
    ThrowIfFailed(command_list_->Close());
    ID3D12CommandList* command_lists[] = { command_list_.Get() };
    command_queue_->ExecuteCommandLists(_countof(command_lists), command_lists);

    // Wait until resize is complete.
    FlushCommandQueue();

    // Update the viewport transform to cover the client area.
    screen_viewport_.TopLeftX = 0;
    screen_viewport_.TopLeftY = 0;
    screen_viewport_.Width = static_cast<float>(width);
    screen_viewport_.Height = static_cast<float>(height);
    screen_viewport_.MinDepth = 0.0f;
    screen_viewport_.MaxDepth = 1.0f;

    scissor_rect_ = { 0, 0, (LONG)width, (LONG)height };
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  RenderSystem
//--------------------------------------------------------------------------------
RenderSystemDirectX12::RenderSystemDirectX12()
    : RenderSystem(kDirectX12)
    , current_fence_(0)
    , current_back_buffer_(0)
    , rtv_descriptor_size_(0)
    , dsv_descriptor_size_(0)
    , cbv_srv_uav_descriptor_size_(0)
    , driver_type_(D3D_DRIVER_TYPE_HARDWARE)
    , back_buffer_format_(DXGI_FORMAT_R8G8B8A8_UNORM)
    , depth_stencil_format_(DXGI_FORMAT_D24_UNORM_S8_UINT)
    , feature_level_(D3D_FEATURE_LEVEL_11_0)
{
}

//--------------------------------------------------------------------------------
//  ~RenderSystem
//--------------------------------------------------------------------------------
RenderSystemDirectX12::~RenderSystemDirectX12()
{
}

//--------------------------------------------------------------------------------
//  initialize the instance
//  Return：true when succeeded, else false
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  初期化処理
//  戻り値：成功したらtrue、失敗したらfalse
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  初始化
//  返回值：成功则返回true、反之返回false
//--------------------------------------------------------------------------------
bool RenderSystemDirectX12::Initialize()
{
#if defined(DEBUG) || defined(_DEBUG) 
    // Enable the D3D12 debug layer.
    ComPtr<ID3D12Debug> debug_controller;
    ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)));
    debug_controller->EnableDebugLayer();
#endif

    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory_)));
    CreateDevice();
    ThrowIfFailed(device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_)));
    SaveDescriptorSize();
    CheckMsaaQuality();
    CreateCommandObjects();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();
    OnResize();
    return true;
}

//--------------------------------------------------------------------------------
//  Uninit the instance
//  インスタンスの終了処理
//  终了处理
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::Uninitialize()
{
    
}

//--------------------------------------------------------------------------------
//  Prepare for render components
//  Return：true when succeeded, else false
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  描画する前の準備
//  戻り値：成功したらtrue、失敗したらfalse
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  渲染前的准备工作
//  返回值：成功则返回true、反之返回false
//--------------------------------------------------------------------------------
bool RenderSystemDirectX12::PrepareRender()
{
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    // 重复使用记录命令的相关内存
    // 只有当与GPU关联的命令列表执行完成时，才能将其重置
    ThrowIfFailed(command_list_allocator_->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    // 将通过ExecuteCommandList方法将某个命令列表加入命令队列后，便可重置该命令
    // 以此来复用命令列表及其内存
    ThrowIfFailed
    (
        command_list_->Reset
        (
            command_list_allocator_.Get(),
            nullptr
        )
    );

    // Indicate a state transition on the resource usage.
    // 对资源的状态进行转换，将资源从呈现状态转换为渲染目标状态
    command_list_->ResourceBarrier
    (
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition
        (
            CurrentBackBuffer(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET
        )
    );

    // Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
    // 设置视口和裁剪矩形，它们需要随着命令列表的重置而重置
    command_list_->RSSetViewports
    (
        1, 
        &screen_viewport_
    );

    command_list_->RSSetScissorRects
    (
        1,
        &scissor_rect_
    );

    // Clear the back buffer and depth buffer.
    // 清除后台缓冲区和深度缓冲区
    command_list_->ClearRenderTargetView
    (
        CurrentBackBufferView(),
        background_color_, 
        0, 
        nullptr
    );

    command_list_->ClearDepthStencilView
    (
        DepthStencilView(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        1.0f,
        0,
        0,
        nullptr
    );

    // Specify the buffers we are going to render to.
    // 指定将要渲染的缓冲区
    command_list_->OMSetRenderTargets
    (
        1, 
        &CurrentBackBufferView(),
        true,
        &DepthStencilView()
    );

    // Indicate a state transition on the resource usage.
    // 再次对资源状态进行转换，将资源从渲染目标状态转换回呈现状态
    command_list_->ResourceBarrier
    (
        1, 
        &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, 
        D3D12_RESOURCE_STATE_PRESENT)
    );

    // Done recording commands.
    // 完成命令记录
    ThrowIfFailed(command_list_->Close());

    // Add the command list to the queue for execution.
    // 将待执行的命令列表加入命令队列
    ID3D12CommandList* command_lists[] = { command_list_.Get() };
    command_queue_->ExecuteCommandLists(_countof(command_lists), command_lists);

    return true;
}

//--------------------------------------------------------------------------------
//  End render and present the buffers 
//  描画終了、バッファの切り替え
//  渲染完成并交换画面缓存
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::EndRender()
{
    // swap the back and front buffers
    // 交换前台与后台缓冲区
    ThrowIfFailed(swap_chain_->Present(0, 0));
    current_back_buffer_ = (current_back_buffer_ + 1) % kSwapChainBufferCount;

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    // 等待此前的命令执行完毕，当前的实现没有什么效果，也过于简单
    // 在后面将重新组织渲染部分的代码，以免在每一帧都要等待
    FlushCommandQueue();
}

//--------------------------------------------------------------------------------
//  Create the directx12 device
//  デバイスの生成
//  生成DirectX12的Device
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CreateDevice()
{
    // Try to create hardware device.
    HRESULT hardware_result = D3D12CreateDevice
    (
        nullptr,                // default adapter
        feature_level_,
        IID_PPV_ARGS(&device_)
    );

    // Fallback to WARP device.
    if (FAILED(hardware_result))
    {
        ComPtr<IDXGIAdapter> warp_adapter;

        ThrowIfFailed(factory_->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)));

        ThrowIfFailed
        (
            D3D12CreateDevice
            (
                warp_adapter.Get(),
                feature_level_,
                IID_PPV_ARGS(&device_)
            )
        );
    }
}

//--------------------------------------------------------------------------------
//  Save the descriptor sizes
//  descriptor sizeの保存
//  保存descriptor size
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::SaveDescriptorSize()
{
    rtv_descriptor_size_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    dsv_descriptor_size_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    cbv_srv_uav_descriptor_size_ = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

//--------------------------------------------------------------------------------
//  Check MSAA quality support
//  MSAA quality supportの確認
//  确认是否支持抗锯齿及其质量
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CheckMsaaQuality()
{
    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render 
    // target formats, so we only need to check quality support.

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS quality_levels;
    quality_levels.Format = back_buffer_format_;
    quality_levels.SampleCount = 4;
    quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    quality_levels.NumQualityLevels = 0;

    ThrowIfFailed
    (
        device_->CheckFeatureSupport
        (
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &quality_levels,
            sizeof(quality_levels)
        )
    );

    msaa_quality_ = quality_levels.NumQualityLevels;
    assert(msaa_quality_ > 0 && "Unexpected MSAA quality level.");
}

//--------------------------------------------------------------------------------
//  Create the descriptor heap of render target view and depth stencil view
//  RtvとDsvのdescriptor heapを作成
//  生成Rtv和Dsv的descriptor heap/描述符堆
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
    rtv_heap_desc.NumDescriptors = kSwapChainBufferCount;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtv_heap_desc.NodeMask = 0;
    ThrowIfFailed
    (
        device_->CreateDescriptorHeap
        (
            &rtv_heap_desc,
            IID_PPV_ARGS(rtv_heap_.GetAddressOf())
        )
    );

    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
    dsv_heap_desc.NumDescriptors = 1;
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsv_heap_desc.NodeMask = 0;
    ThrowIfFailed
    (
        device_->CreateDescriptorHeap
        (
            &dsv_heap_desc, 
            IID_PPV_ARGS(dsv_heap_.GetAddressOf())
        )
    );
}

//--------------------------------------------------------------------------------
//  Create the command queue, command allocator and command list
//  command queue, command allocator, command listの作成
//  生成command queue, command allocator, command list
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    ThrowIfFailed(device_->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&command_queue_)));

    ThrowIfFailed
    (
        device_->CreateCommandAllocator
        (
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(command_list_allocator_.GetAddressOf())
        )
    );

    ThrowIfFailed
    (
        device_->CreateCommandList
        (
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            command_list_allocator_.Get(), // Associated command allocator
            nullptr, // Initial PipelineStateObject
            IID_PPV_ARGS(command_list_.GetAddressOf())
        )
    );

    // Start off in a closed state.  This is because the first time we refer 
    // to the command list we will Reset it, and it needs to be closed before
    // calling Reset.
    command_list_->Close();
}

//--------------------------------------------------------------------------------
//  Create the swap chain
//  swap chainの生成
//  生成swap chain
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CreateSwapChain()
{
    MainSystem& main_system = MainSystem::Instance();

    // Release the previous swapchain we will be recreating.
    swap_chain_.Reset();

    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    swap_chain_desc.BufferDesc.Width = main_system.Width();
    swap_chain_desc.BufferDesc.Height = main_system.Height();
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_desc.BufferDesc.Format = back_buffer_format_;
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.SampleDesc.Count = msaa_enable_ ? 4 : 1;
    swap_chain_desc.SampleDesc.Quality = msaa_enable_ ? (msaa_quality_ - 1) : 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = kSwapChainBufferCount;
    swap_chain_desc.OutputWindow = main_system.MainWindowHandle();
    swap_chain_desc.Windowed = true;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Note: Swap chain uses queue to perform flush.
    // swapchain需要通过commandqueue对其进行刷新
    ThrowIfFailed
    (
        factory_->CreateSwapChain
        (
            command_queue_.Get(),
            &swap_chain_desc,
            swap_chain_.GetAddressOf()
        )
    );
}

//--------------------------------------------------------------------------------
//  Upload to command queue
//  command queueにアップロードする
//  上传命令至command queue
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::FlushCommandQueue()
{
    // Advance the fence value to mark commands up to this fence point.
    current_fence_++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    ThrowIfFailed(command_queue_->Signal(fence_.Get(), current_fence_));

    // Wait until the GPU has completed commands up to this fence point.
    if (fence_->GetCompletedValue() < current_fence_)
    {
        HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

        // Fire event when GPU hits current fence.  
        ThrowIfFailed(fence_->SetEventOnCompletion(current_fence_, event_handle));

        // Wait until the GPU hits current fence event is fired.
        WaitForSingleObject(event_handle, INFINITE);
        CloseHandle(event_handle);
    }
}

//--------------------------------------------------------------------------------
//  Create the render target view.
//  render target viewの作成
//  为swapchain的每一个buffer创建RTV
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CreateRenderTargetView()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(rtv_heap_->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < kSwapChainBufferCount; i++)
    {
        // 获得存于swapchain中的buffer
        ThrowIfFailed
        (
            swap_chain_->GetBuffer
            (
                i, // 希望获得的特定backbuffer的index
                IID_PPV_ARGS(&swap_chain_buffer_[i])
            )
        );

        // 为获取的backbuffer创建rtv
        device_->CreateRenderTargetView
        (
            swap_chain_buffer_[i].Get(),
            nullptr, // 指向D3D12_RENDER_TARGET_VIEW_DESC数据结构的pointer
                     // 如果该资源在创建时已指定了具体格式(not typeless)
                     // 那么就可以把这个参数设为nullptr，表示采用该资源创建时的格式
                     // 为它的第一个mipmap层级创建一个视图
            rtv_heap_handle
        );

        // 偏移到下一个buffer
        rtv_heap_handle.Offset
        (
            1,
            rtv_descriptor_size_
        );
    }
}

//--------------------------------------------------------------------------------
//  Create the depth/stencil buffer and view.
//  depth/stencil buffer and viewの作成
//  创建深度/模板缓冲区及视图
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CreateDepthStencilBufferView(const UINT width, const UINT height)
{
    D3D12_RESOURCE_DESC depth_stencil_desc;
    depth_stencil_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 资源的维度
    depth_stencil_desc.Alignment = 0;
    depth_stencil_desc.Width = width;
    depth_stencil_desc.Height = height;
    depth_stencil_desc.DepthOrArraySize = 1;
    depth_stencil_desc.MipLevels = 1;

    // Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
    // the depth buffer.  Therefore, because we need to create two views to the same resource:
    //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
    //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
    // we need to create the depth buffer resource with a typeless format.  
    depth_stencil_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depth_stencil_desc.SampleDesc.Count = msaa_enable_ ? 4 : 1;
    depth_stencil_desc.SampleDesc.Quality = msaa_enable_ ? (msaa_quality_ - 1) : 0;
    depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depth_stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE opt_clear;
    opt_clear.Format = depth_stencil_format_;
    opt_clear.DepthStencil.Depth = 1.0f;
    opt_clear.DepthStencil.Stencil = 0;

    ThrowIfFailed
    (
        device_->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &depth_stencil_desc,
            D3D12_RESOURCE_STATE_COMMON,
            &opt_clear,
            IID_PPV_ARGS(depth_stencil_buffer_.GetAddressOf())
        )
    );

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Format = depth_stencil_format_;
    dsv_desc.Texture2D.MipSlice = 0;
    device_->CreateDepthStencilView
    (
        depth_stencil_buffer_.Get(),
        &dsv_desc,
        DepthStencilView()
    );

    // Transition the resource from its initial state to be used as a depth buffer.
    // 将资源从初始状态转换为深度缓冲区
    // 资源转换，防止资源冒险(resource hazard) 书p100
    command_list_->ResourceBarrier
    (
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(depth_stencil_buffer_.Get(),
            D3D12_RESOURCE_STATE_COMMON,
            D3D12_RESOURCE_STATE_DEPTH_WRITE)
    );
}

//--------------------------------------------------------------------------------
//  Get the current backbuffer
//  Return：ID3D12Resource*
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  今のバックバッファを取得する
//  戻り値：ID3D12Resource*
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  返回现在的备用缓冲区
//  返回值：ID3D12Resource*
//--------------------------------------------------------------------------------
ID3D12Resource* RenderSystemDirectX12::CurrentBackBuffer() const
{
    return swap_chain_buffer_[current_back_buffer_].Get();
}

//--------------------------------------------------------------------------------
//  Get the current backbuffer view
//  Return：D3D12_CPU_DESCRIPTOR_HANDLE
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  今のバックバッファビューを取得する
//  戻り値：D3D12_CPU_DESCRIPTOR_HANDLE
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  返回现在的备用缓冲视图
//  返回值：D3D12_CPU_DESCRIPTOR_HANDLE
//--------------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE RenderSystemDirectX12::CurrentBackBufferView() const
{
    // 根据给定的偏移量找到当前后台缓冲区的RTV
    return CD3DX12_CPU_DESCRIPTOR_HANDLE
    (
        rtv_heap_->GetCPUDescriptorHandleForHeapStart(), // heap中的首个handle
        current_back_buffer_,
        rtv_descriptor_size_
    );
}

//--------------------------------------------------------------------------------
//  Get the depth stencil view
//  Return：D3D12_CPU_DESCRIPTOR_HANDLE
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  深度情報ビューを取得する
//  戻り値：D3D12_CPU_DESCRIPTOR_HANDLE
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  返回深度缓存视图
//  返回值：D3D12_CPU_DESCRIPTOR_HANDLE
//--------------------------------------------------------------------------------
D3D12_CPU_DESCRIPTOR_HANDLE RenderSystemDirectX12::DepthStencilView() const
{
    return dsv_heap_->GetCPUDescriptorHandleForHeapStart();
}
