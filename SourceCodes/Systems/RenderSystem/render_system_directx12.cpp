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
#include "../main_system.h"
#include "../../Utilities/kf_utilities.h"
#include "../../Utilities/exception.h"
#include "../../Libraries/DirectX12/d3dx12.h"
#include <array>
#include <vector>
using namespace KeepFortissimo;
using namespace DirectX;
using namespace std;

// Test
#include <D3Dcompiler.h>

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  OnResize
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::OnResize()
{
    assert(m_device);
    assert(m_swap_chain);
    assert(m_command_list_allocator);

    UINT width = MainSystem::Instance().Width();
    UINT height = MainSystem::Instance().Height();

    // Flush before changing any resources.
    FlushCommandQueue();

    ThrowIfFailed(m_command_list->Reset(m_command_list_allocator.Get(), nullptr));

    // Release the previous resources we will be recreating.
    for (int i = 0; i < sc_swap_chain_buffer_count; ++i)
    {
        m_swap_chain_buffer[i].Reset();
    }

    m_depth_stencil_buffer.Reset();

    // Resize the swap chain.
    ThrowIfFailed
    (
        m_swap_chain->ResizeBuffers
        (
            sc_swap_chain_buffer_count,
            width,
            height,
            m_back_buffer_format,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
        )
    );

    m_current_back_buffer = 0;

    // 为swapchain的每一个buffer创建RTV
    CreateRenderTargetView();

    // Create the depth/stencil buffer and view.
    // 创建深度/模板缓冲区及视图
    CreateDepthStencilBufferView(width, height);

    // Execute the resize commands.
    ThrowIfFailed(m_command_list->Close());
    ID3D12CommandList* command_lists[] = { m_command_list.Get() };
    m_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

    // Wait until resize is complete.
    FlushCommandQueue();

    // Update the viewport transform to cover the client area.
    m_screen_viewport.TopLeftX = 0;
    m_screen_viewport.TopLeftY = 0;
    m_screen_viewport.Width = static_cast<float>(width);
    m_screen_viewport.Height = static_cast<float>(height);
    m_screen_viewport.MinDepth = 0.0f;
    m_screen_viewport.MaxDepth = 1.0f;

    m_scissor_rect = { 0, 0, static_cast<LONG>(width), static_cast<LONG>(height) };
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
    : RenderSystem(RenderApiType::kDirectX12)
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

    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)));
    CreateDevice();
    ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    SaveDescriptorSize();
    CheckMsaaQuality();
    CreateCommandObjects();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();
    OnResize();
    InitBox();
    return true;
}

//--------------------------------------------------------------------------------
//  Uninit the instance
//  インスタンスの終了処理
//  终了处理
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::Uninitialize()
{
    UninitBox();
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
    //// Reuse the memory associated with command recording.
    //// We can only reset when the associated command lists have finished execution on the GPU.
    //// 重复使用记录命令的相关内存
    //// 只有当与GPU关联的命令列表执行完成时，才能将其重置
    //ThrowIfFailed(m_command_list_allocator->Reset());

    //// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    //// Reusing the command list reuses memory.
    //// 将通过ExecuteCommandList方法将某个命令列表加入命令队列后，便可重置该命令
    //// 以此来复用命令列表及其内存
    //ThrowIfFailed
    //(
    //    m_command_list->Reset
    //    (
    //        m_command_list_allocator.Get(),
    //        nullptr
    //    )
    //);

    //// Indicate a state transition on the resource usage.
    //// 对资源的状态进行转换，将资源从呈现状态转换为渲染目标状态
    //m_command_list->ResourceBarrier
    //(
    //    1,
    //    &CD3DX12_RESOURCE_BARRIER::Transition
    //    (
    //        CurrentBackBuffer(),
    //        D3D12_RESOURCE_STATE_PRESENT,
    //        D3D12_RESOURCE_STATE_RENDER_TARGET
    //    )
    //);

    //// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
    //// 设置视口和裁剪矩形，它们需要随着命令列表的重置而重置
    //m_command_list->RSSetViewports
    //(
    //    1, 
    //    &m_screen_viewport
    //);

    //m_command_list->RSSetScissorRects
    //(
    //    1,
    //    &m_scissor_rect
    //);

    //// Clear the back buffer and depth buffer.
    //// 清除后台缓冲区和深度缓冲区
    //m_command_list->ClearRenderTargetView
    //(
    //    CurrentBackBufferView(),
    //    m_background_color, 
    //    0, 
    //    nullptr
    //);

    //m_command_list->ClearDepthStencilView
    //(
    //    DepthStencilView(),
    //    D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
    //    1.0f,
    //    0,
    //    0,
    //    nullptr
    //);

    //// Specify the buffers we are going to render to.
    //// 指定将要渲染的缓冲区
    //m_command_list->OMSetRenderTargets
    //(
    //    1, 
    //    &CurrentBackBufferView(),
    //    true,
    //    &DepthStencilView()
    //);

    //// Indicate a state transition on the resource usage.
    //// 再次对资源状态进行转换，将资源从渲染目标状态转换回呈现状态
    //m_command_list->ResourceBarrier
    //(
    //    1, 
    //    &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
    //    D3D12_RESOURCE_STATE_RENDER_TARGET, 
    //    D3D12_RESOURCE_STATE_PRESENT)
    //);

    //// Done recording commands.
    //// 完成命令记录
    //ThrowIfFailed(m_command_list->Close());

    //// Add the command list to the queue for execution.
    //// 将待执行的命令列表加入命令队列
    //ID3D12CommandList* command_lists[] = { m_command_list.Get() };
    //m_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

    UpdateBox();
    DrawBox();

    return true;
}

//--------------------------------------------------------------------------------
//  End render and present the buffers 
//  描画終了、バッファの切り替え
//  渲染完成并交换画面缓存
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::EndRender()
{
    //// swap the back and front buffers
    //// 交换前台与后台缓冲区
    //ThrowIfFailed(m_swap_chain->Present(0, 0));
    //m_current_back_buffer = (m_current_back_buffer + 1) % sc_swap_chain_buffer_count;

    //// Wait until frame commands are complete.  This waiting is inefficient and is
    //// done for simplicity.  Later we will show how to organize our rendering code
    //// so we do not have to wait per frame.
    //// 等待此前的命令执行完毕，当前的实现没有什么效果，也过于简单
    //// 在后面将重新组织渲染部分的代码，以免在每一帧都要等待
    //FlushCommandQueue();
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
        m_feature_level,
        IID_PPV_ARGS(&m_device)
    );

    // Fallback to WARP device.
    if (FAILED(hardware_result))
    {
        ComPtr<IDXGIAdapter> warp_adapter;

        ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warp_adapter)));

        ThrowIfFailed
        (
            D3D12CreateDevice
            (
                warp_adapter.Get(),
                m_feature_level,
                IID_PPV_ARGS(&m_device)
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
    m_rtv_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_dsv_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_cbv_srv_uav_descriptor_size = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
    quality_levels.Format = m_back_buffer_format;
    quality_levels.SampleCount = 4;
    quality_levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    quality_levels.NumQualityLevels = 0;

    ThrowIfFailed
    (
        m_device->CheckFeatureSupport
        (
            D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
            &quality_levels,
            sizeof(quality_levels)
        )
    );

    m_msaa_quality = quality_levels.NumQualityLevels;
    assert(m_msaa_quality > 0 && "Unexpected MSAA quality level.");
}

//--------------------------------------------------------------------------------
//  Create the descriptor heap of render target view and depth stencil view
//  RtvとDsvのdescriptor heapを作成
//  生成Rtv和Dsv的descriptor heap/描述符堆
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc;
    rtv_heap_desc.NumDescriptors = sc_swap_chain_buffer_count;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtv_heap_desc.NodeMask = 0;
    ThrowIfFailed
    (
        m_device->CreateDescriptorHeap
        (
            &rtv_heap_desc,
            IID_PPV_ARGS(m_rtv_heap.GetAddressOf())
        )
    );

    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
    dsv_heap_desc.NumDescriptors = 1;
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsv_heap_desc.NodeMask = 0;
    ThrowIfFailed
    (
        m_device->CreateDescriptorHeap
        (
            &dsv_heap_desc, 
            IID_PPV_ARGS(m_dsv_heap.GetAddressOf())
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
    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_command_queue)));

    ThrowIfFailed
    (
        m_device->CreateCommandAllocator
        (
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(m_command_list_allocator.GetAddressOf())
        )
    );

    ThrowIfFailed
    (
        m_device->CreateCommandList
        (
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_command_list_allocator.Get(), // Associated command allocator
            nullptr, // Initial PipelineStateObject
            IID_PPV_ARGS(m_command_list.GetAddressOf())
        )
    );

    // Start off in a closed state.  This is because the first time we refer 
    // to the command list we will Reset it, and it needs to be closed before
    // calling Reset.
    m_command_list->Close();
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
    m_swap_chain.Reset();

    DXGI_SWAP_CHAIN_DESC swap_chain_desc;
    swap_chain_desc.BufferDesc.Width = main_system.Width();
    swap_chain_desc.BufferDesc.Height = main_system.Height();
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
    swap_chain_desc.BufferDesc.Format = m_back_buffer_format;
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_desc.SampleDesc.Count = m_msaa_enable ? 4 : 1;
    swap_chain_desc.SampleDesc.Quality = m_msaa_enable ? (m_msaa_quality - 1) : 0;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = sc_swap_chain_buffer_count;
    swap_chain_desc.OutputWindow = main_system.MainWindowHandle();
    swap_chain_desc.Windowed = true;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // Note: Swap chain uses queue to perform flush.
    // swapchain需要通过commandqueue对其进行刷新
    ThrowIfFailed
    (
        m_factory->CreateSwapChain
        (
            m_command_queue.Get(),
            &swap_chain_desc,
            m_swap_chain.GetAddressOf()
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
    m_current_fence++;

    // Add an instruction to the command queue to set a new fence point.  Because we 
    // are on the GPU timeline, the new fence point won't be set until the GPU finishes
    // processing all the commands prior to this Signal().
    ThrowIfFailed(m_command_queue->Signal(m_fence.Get(), m_current_fence));

    // Wait until the GPU has completed commands up to this fence point.
    if (m_fence->GetCompletedValue() < m_current_fence)
    {
        HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

        // Fire event when GPU hits current fence.  
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_current_fence, event_handle));

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
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_heap_handle(m_rtv_heap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < sc_swap_chain_buffer_count; i++)
    {
        // 获得存于swapchain中的buffer
        ThrowIfFailed
        (
            m_swap_chain->GetBuffer
            (
                i, // 希望获得的特定backbuffer的index
                IID_PPV_ARGS(&m_swap_chain_buffer[i])
            )
        );

        // 为获取的backbuffer创建rtv
        m_device->CreateRenderTargetView
        (
            m_swap_chain_buffer[i].Get(),
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
            m_rtv_descriptor_size
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
    depth_stencil_desc.SampleDesc.Count = m_msaa_enable ? 4 : 1;
    depth_stencil_desc.SampleDesc.Quality = m_msaa_enable ? (m_msaa_quality - 1) : 0;
    depth_stencil_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depth_stencil_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE opt_clear;
    opt_clear.Format = m_depth_stencil_format;
    opt_clear.DepthStencil.Depth = 1.0f;
    opt_clear.DepthStencil.Stencil = 0;

    ThrowIfFailed
    (
        m_device->CreateCommittedResource
        (
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &depth_stencil_desc,
            D3D12_RESOURCE_STATE_COMMON,
            &opt_clear,
            IID_PPV_ARGS(m_depth_stencil_buffer.GetAddressOf())
        )
    );

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc;
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Format = m_depth_stencil_format;
    dsv_desc.Texture2D.MipSlice = 0;
    m_device->CreateDepthStencilView
    (
        m_depth_stencil_buffer.Get(),
        &dsv_desc,
        DepthStencilView()
    );

    // Transition the resource from its initial state to be used as a depth buffer.
    // 将资源从初始状态转换为深度缓冲区
    // 资源转换，防止资源冒险(resource hazard) 书p100
    m_command_list->ResourceBarrier
    (
        1,
        &CD3DX12_RESOURCE_BARRIER::Transition(m_depth_stencil_buffer.Get(),
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
    return m_swap_chain_buffer[m_current_back_buffer].Get();
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
        m_rtv_heap->GetCPUDescriptorHandleForHeapStart(), // heap中的首个handle
        m_current_back_buffer,
        m_rtv_descriptor_size
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
    return m_dsv_heap->GetCPUDescriptorHandleForHeapStart();
}

//Test
void RenderSystemDirectX12::InitBox()
{
    // Reset the command list to prep for initialization commands.
    ThrowIfFailed(m_command_list->Reset(m_command_list_allocator.Get(), nullptr));

    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
    BuildBoxGeometry();
    BuildPSO();

    // Execute the initialization commands.
    ThrowIfFailed(m_command_list->Close());
    ID3D12CommandList* command_lists[] = { m_command_list.Get() };
    m_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

    // Wait until initialization is complete.
    FlushCommandQueue();
}

void RenderSystemDirectX12::UninitBox()
{
    SAFE_DELETE(mObjectCB);
    SAFE_DELETE(mBoxGeo);
}

void RenderSystemDirectX12::UpdateBox()
{
    XMMATRIX mProj =  XMMatrixIdentity();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;

    // Convert Spherical to Cartesian coordinates.
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);
    float y = mRadius * cosf(mPhi);

    // Build the view matrix.
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

    XMMATRIX world = XMMatrixIdentity();
    float width = static_cast<float>(MainSystem::Instance().Width());
    float height = static_cast<float>(MainSystem::Instance().Height());
    XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f * XM_PI, width / height, 1.0f, 1000.0f);
    XMMATRIX worldViewProj = world * view * proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    ObjectConstants objConstants;
    objConstants.world_view_projection = XMMatrixTranspose(worldViewProj);
    mObjectCB->CopyData(0, objConstants);
}

void RenderSystemDirectX12::DrawBox()
{
    // Reuse the memory associated with command recording.
// We can only reset when the associated command lists have finished execution on the GPU.
    ThrowIfFailed(m_command_list_allocator->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(m_command_list->Reset(m_command_list_allocator.Get(), mPSO.Get()));

    m_command_list->RSSetViewports(1, &m_screen_viewport);
    m_command_list->RSSetScissorRects(1, &m_scissor_rect);

    // Indicate a state transition on the resource usage.
    m_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Clear the back buffer and depth buffer.
    m_command_list->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    m_command_list->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    m_command_list->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    /////////////////////////////////////////////////////////////////////////////
    // 先将根签名和cbv堆设置到命令列表上，并随后再通过设置描述符表来制定我们
    // 希望绑定到渲染流水线的资源
    ID3D12DescriptorHeap* descriptor_heaps[] = { mCbvHeap.Get() };
    m_command_list->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);

    m_command_list->SetGraphicsRootSignature(mRootSignature.Get());

    m_command_list->SetGraphicsRootDescriptorTable(
        0, // 将跟参数按此索引（即与绑定到的寄存器槽号）进行设置
        // 此参数指定的是将要想着色器绑定的描述符表中第一个描述符位于描述符
        // 堆中的句柄。比如说，如果根签名知名当前描述符表中共有5个描述符，
        // 则堆中的BaseDescriptor及其后面的4个描述符将被设置到此描述符表中
        mCbvHeap->GetGPUDescriptorHandleForHeapStart());
    //
    /////////////////////////////////////////////////////////////////////////////

    m_command_list->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());
    m_command_list->IASetIndexBuffer(&mBoxGeo->IndexBufferView());
    m_command_list->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



    m_command_list->DrawIndexedInstanced(
        mBoxGeo->unit_mesh_geometries["box"].index_count,
        1, 0, 0, 0);

    // Indicate a state transition on the resource usage.
    m_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    // Done recording commands.
    ThrowIfFailed(m_command_list->Close());

    // Add the command list to the queue for execution.
    ID3D12CommandList* command_lists[] = { m_command_list.Get() };
    m_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

    // swap the back and front buffers
    ThrowIfFailed(m_swap_chain->Present(0, 0));
    m_current_back_buffer = (m_current_back_buffer + 1) % sc_swap_chain_buffer_count;

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    FlushCommandQueue();
}

// 利用描述符将常量缓冲区绑定到渲染流水线上
void RenderSystemDirectX12::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&mCbvHeap)));
}

void RenderSystemDirectX12::BuildConstantBuffers()
{
    mObjectCB = MY_NEW UploadBuffer<ObjectConstants>(m_device.Get(), 1, true);

    const u32 object_constant_buffer_size = Utility::CalculateConstantBufferByteSize(sizeof(ObjectConstants));

    // 缓冲区的起始地址（即索引为0的那个常量缓冲区的地址）
    D3D12_GPU_VIRTUAL_ADDRESS gpu_virtual_address = mObjectCB->Resource()->GetGPUVirtualAddress();
   
    // 偏移到常量缓冲区中绘制第一个物体所需的常量数据
    // Offset to the ith object constant buffer in the buffer.
    int buffer_index = 0;
    gpu_virtual_address += buffer_index * object_constant_buffer_size;

    D3D12_CONSTANT_BUFFER_VIEW_DESC constant_buffer_view_desc;
    constant_buffer_view_desc.BufferLocation = gpu_virtual_address;
    constant_buffer_view_desc.SizeInBytes = object_constant_buffer_size;

    m_device->CreateConstantBufferView(
        &constant_buffer_view_desc,
        mCbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void RenderSystemDirectX12::BuildRootSignature()
{
    // 这段代码创建了一个跟参数，目的是将含有一个CBV的描述符绑定到常量缓冲区寄存器0
    // 即register(b0)
    // Shader programs typically require resources as input (constant buffers,
    // textures, samplers).  The root signature defines the resources the shader
    // programs expect.  If we think of the shader programs as a function, and
    // the input resources as function parameters, then the root signature can be
    // thought of as defining the function signature.  

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slot_root_parameter[1];

    // Create a single descriptor table of CBVs.(constant buffer view)
    CD3DX12_DESCRIPTOR_RANGE cbv_table;
    cbv_table.Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,// 描述符表的类型
        1,                              // 表中的描述符数量
        0);                             // 将这段描述符区域绑定至此基准着色器寄存器（base shader register）

    slot_root_parameter[0].InitAsDescriptorTable(
        1,          // 描述符区域的数量
        &cbv_table);// 只想描述符区域数组的指针

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC root_signature_desc(1, slot_root_parameter, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // 创建仅含一个槽位（该槽位指向一个仅由单个常量缓冲区组成的描述符区域）的根签名
    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serialized_root_signature = nullptr;
    ComPtr<ID3DBlob> error_blob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(
        &root_signature_desc,
        D3D_ROOT_SIGNATURE_VERSION_1, 
        serialized_root_signature.GetAddressOf(),
        error_blob.GetAddressOf());

    if (error_blob != nullptr)
    {
        ::OutputDebugStringA((char*)error_blob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(m_device->CreateRootSignature(
        0,
        serialized_root_signature->GetBufferPointer(),
        serialized_root_signature->GetBufferSize(),
        IID_PPV_ARGS(&mRootSignature)));
}

void RenderSystemDirectX12::BuildBoxGeometry()
{
    std::array<Vertex3d, 8> vertices =
    {
        Vertex3d({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
        Vertex3d({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
        Vertex3d({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
        Vertex3d({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
        Vertex3d({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
        Vertex3d({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
        Vertex3d({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
        Vertex3d({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
    };

    std::array<std::uint16_t, 36> indices =
    {
        // front face
        0, 1, 2,
        0, 2, 3,

        // back face
        4, 6, 5,
        4, 7, 6,

        // left face
        4, 5, 1,
        4, 1, 0,

        // right face
        3, 2, 6,
        3, 6, 7,

        // top face
        1, 5, 6,
        1, 6, 2,

        // bottom face
        4, 0, 3,
        4, 3, 7
    };

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex3d);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    mBoxGeo = MY_NEW MeshGeometry;
    mBoxGeo->name = "boxGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->vertex_buffer_cpu));
    CopyMemory(mBoxGeo->vertex_buffer_cpu->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->index_buffer_cpu));
    CopyMemory(mBoxGeo->index_buffer_cpu->GetBufferPointer(), indices.data(), ibByteSize);

    mBoxGeo->vertex_buffer_gpu = CreateDefaultBuffer(vertices.data(), vbByteSize, mBoxGeo->vertex_buffer_uploader);

    mBoxGeo->index_buffer_gpu = CreateDefaultBuffer(indices.data(), ibByteSize, mBoxGeo->index_buffer_uploader);

    mBoxGeo->vertex_byte_stride = sizeof(Vertex3d);
    mBoxGeo->vertex_buffer_byte_size = vbByteSize;
    mBoxGeo->index_format = DXGI_FORMAT_R16_UINT;
    mBoxGeo->index_buffer_byte_size = ibByteSize;

    UnitMeshGeometry unit_mesh;
    unit_mesh.index_count = (UINT)indices.size();
    unit_mesh.start_index_location = 0;
    unit_mesh.base_vertex_location = 0;

    mBoxGeo->unit_mesh_geometries["box"] = unit_mesh;
}

void RenderSystemDirectX12::BuildPSO()
{
    // BuildShadersAndInputLayout
    mvsByteCode = CompileShader(L"SourceCodes\\Systems\\RenderSystem\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
    mpsByteCode = CompileShader(L"SourceCodes\\Systems\\RenderSystem\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // BuildPSO
    HRESULT hr = S_OK;

    //--------------------------------------------------------------------------------
    // 流水线状态对象(pipeline state object) 书P215
    // PSO的验证和创建操作过于耗时，所以应在初始化期间就生成PSO，除非有特别需求
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

    // 指向一个与此pso像绑定的根签名指针。该根签名一定要于此PSO指定的着色器相兼容
    psoDesc.pRootSignature = mRootSignature.Get();

    // 输入布局描述
    psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };

    // 待绑定的顶点着色器，由结构体D3D12_SHADER_BYTECODE表示
    // 这个结构体存有指向已编译好的字节码数据的指针
    // 以及该字节码数据所占的字节大小
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
        mvsByteCode->GetBufferSize()
    };

    // 待绑定的像素着色器
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
        mpsByteCode->GetBufferSize()
    };

    // 指定用来配置光栅器的光栅化状态
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    // 指定混合操作所用的混合状态
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    // 指定用于配置深度/模板状态。
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    // 多重采样最多可采集32个样本。借此参数的32位整数值，即可设置每个采样点的采集情况
    // （采集或禁止采集）。例如若禁用了第五位（将第五位设置为0），则将不会对第五个样本
    // 进行采样。当然，要禁止采集第五个样本的前提是，所用的多重采样至少要有五个样本。
    // 假如一个应用程序仅使用了单采样（single sampling），那么只能针对该参数的第一位
    // 进行配置。一般来说，使用的都是默认值0xffffffff，即表示对所有的采样点都进行采样
    psoDesc.SampleMask = UINT_MAX;

    // 指定图元的拓扑类型
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    // 同时所用的渲染目标数量（即RTVFormats数组中渲染目标格式的数量）
    psoDesc.NumRenderTargets = 1;

    // 渲染目标的格式。利用该数组实现向多渲染目标同时进行写操作
    psoDesc.RTVFormats[0] = m_back_buffer_format;

    // 描述多重采样对每个像素采样的数量及其质量级别
    psoDesc.SampleDesc.Count = m_msaa_enable ? 4 : 1;
    psoDesc.SampleDesc.Quality = m_msaa_enable ? (m_msaa_quality - 1) : 0;

    // 深度/模板缓冲区的格式
    psoDesc.DSVFormat = m_depth_stencil_format;
    //
    //--------------------------------------------------------------------------------

    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}

// 利用作为中介的上传缓冲区来初始化默认缓冲区
ComPtr<ID3D12Resource> RenderSystemDirectX12::CreateDefaultBuffer(const void* init_data, u64 byte_size, ComPtr<ID3D12Resource>& upload_buffer)
{
    ComPtr<ID3D12Resource> default_buffer;

    // 创建实际的默认缓冲区资源
    // Create the actual default buffer resource.
    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byte_size),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(default_buffer.GetAddressOf())));

    // 为了将cpu端内存中的数据复制到默认缓冲区，我们还需要创建一个处于中介位置的上传堆
    // In order to copy CPU memory data into our default buffer, we need to create
    // an intermediate upload heap. 
    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(byte_size),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(upload_buffer.GetAddressOf())));

    // 描述我们希望复制到默认缓冲区中的数据
    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA sub_resource_data = {};
    sub_resource_data.pData = init_data;    // 指向某个系统内存块的指针
    sub_resource_data.RowPitch = byte_size; // 数据的字节数
    sub_resource_data.SlicePitch = sub_resource_data.RowPitch; // 数据的字节数

    // 将数据复制到默认缓冲区资源的流程
    // UpdateSubresources函数会先将数据从cpu端的内存中复制到位于中介位置的上传堆里
    // 接着再通过调用ID3D12CommandList::CopySubresourceRegion函数，把上传堆内的数据复制到mBuffer
    // Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
    // will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
    // the intermediate upload heap data will be copied to mBuffer.
    m_command_list->ResourceBarrier(1, 
        &CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(),
            D3D12_RESOURCE_STATE_COMMON, 
            D3D12_RESOURCE_STATE_COPY_DEST));
    UpdateSubresources<1>(m_command_list.Get(), 
        default_buffer.Get(), 
        upload_buffer.Get(), 
        0, 0, 1, 
        &sub_resource_data);
    m_command_list->ResourceBarrier(1, 
        &CD3DX12_RESOURCE_BARRIER::Transition(default_buffer.Get(),
            D3D12_RESOURCE_STATE_COPY_DEST, 
            D3D12_RESOURCE_STATE_GENERIC_READ));

    // 注意；在调用上述函数后，必须保证upload_buffer依然存在，而不能对他立即销毁，
    // 这是因为命令列表中的复制操作可能尚未执行，待调用者得知复制完成的消息后，再释放
    // Note: uploadBuffer has to be kept alive after the above function calls because
    // the command list has not been executed yet that performs the actual copy.
    // The caller can Release the uploadBuffer after it knows the copy has been executed.

    return default_buffer;
}

ComPtr<ID3DBlob> RenderSystemDirectX12::CompileShader(const LPCWSTR filename, const D3D_SHADER_MACRO* defines, const LPCSTR entrypoint, const LPCSTR target)
{
    UINT compile_flags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
    compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    HRESULT hr = S_OK;

    ComPtr<ID3DBlob> byte_code = nullptr;
    ComPtr<ID3DBlob> errors = nullptr;
    hr = D3DCompileFromFile(filename,       // 希望编译的以.hlsl作为扩展名的HLSL源代码文件
        defines,                            // 参见sdk文档
        D3D_COMPILE_STANDARD_FILE_INCLUDE,  // 参见sdk文档
        entrypoint,                         // 着色器入口点函数名，一个.hlsl文件可能存有多个着色器程序
        target,                             // 指定所用着色器类型和版本的字符串
        compile_flags,                      // 指示对着色器代码应当如何编译的标志
        0,                                  // 处理效果文件的高级编译选项
        &byte_code,                         // 返回一个指向ID3DBlob数据结构的指针，存储着编译好的着色器对象字节码
        &errors);                           // 存储报错的字符串

    if (errors != nullptr)
        OutputDebugStringA((char*)errors->GetBufferPointer());

    ThrowIfFailed(hr);

    return byte_code;
}