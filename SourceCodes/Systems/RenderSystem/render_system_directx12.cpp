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
#include "frame_resource.h"
#include "mesh_generator.h"
#include "../main_system.h"
#include "../game_timer.h"
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
//  Render
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::Render()
{
    UpdateTest();
    DrawTest();
    UpdateFrameResource();
}

//--------------------------------------------------------------------------------
//  OnResize
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::OnResize()
{
    assert(m_device);
    assert(m_swap_chain);
    assert(m_command_list_allocator);

    uint32_t width = MainSystem::Instance().Width();
    uint32_t height = MainSystem::Instance().Height();

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
    InitTest();
    return true;
}

//--------------------------------------------------------------------------------
//  Uninit the instance
//  インスタンスの終了処理
//  终了处理
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::Uninitialize()
{
    for (uint32_t i = 0; i < sc_num_frame_resources; ++i)
    {
        SAFE_DELETE(m_frame_resources[i]);
    }
    UninitTest();
}

////--------------------------------------------------------------------------------
////  Prepare for render components
////  Return：true when succeeded, else false
////ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
////  描画する前の準備
////  戻り値：成功したらtrue、失敗したらfalse
////ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
////  渲染前的准备工作
////  返回值：成功则返回true、反之返回false
////--------------------------------------------------------------------------------
//bool RenderSystemDirectX12::PrepareRender()
//{
//    //// Reuse the memory associated with command recording.
//    //// We can only reset when the associated command lists have finished execution on the GPU.
//    //// 重复使用记录命令的相关内存
//    //// 只有当与GPU关联的命令列表执行完成时，才能将其重置
//    //ThrowIfFailed(m_command_list_allocator->Reset());
//
//    //// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
//    //// Reusing the command list reuses memory.
//    //// 将通过ExecuteCommandList方法将某个命令列表加入命令队列后，便可重置该命令
//    //// 以此来复用命令列表及其内存
//    //ThrowIfFailed
//    //(
//    //    m_command_list->Reset
//    //    (
//    //        m_command_list_allocator.Get(),
//    //        nullptr
//    //    )
//    //);
//
//    //// Indicate a state transition on the resource usage.
//    //// 对资源的状态进行转换，将资源从呈现状态转换为渲染目标状态
//    //m_command_list->ResourceBarrier
//    //(
//    //    1,
//    //    &CD3DX12_RESOURCE_BARRIER::Transition
//    //    (
//    //        CurrentBackBuffer(),
//    //        D3D12_RESOURCE_STATE_PRESENT,
//    //        D3D12_RESOURCE_STATE_RENDER_TARGET
//    //    )
//    //);
//
//    //// Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
//    //// 设置视口和裁剪矩形，它们需要随着命令列表的重置而重置
//    //m_command_list->RSSetViewports
//    //(
//    //    1, 
//    //    &m_screen_viewport
//    //);
//
//    //m_command_list->RSSetScissorRects
//    //(
//    //    1,
//    //    &m_scissor_rect
//    //);
//
//    //// Clear the back buffer and depth buffer.
//    //// 清除后台缓冲区和深度缓冲区
//    //m_command_list->ClearRenderTargetView
//    //(
//    //    CurrentBackBufferView(),
//    //    m_background_color, 
//    //    0, 
//    //    nullptr
//    //);
//
//    //m_command_list->ClearDepthStencilView
//    //(
//    //    DepthStencilView(),
//    //    D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
//    //    1.0f,
//    //    0,
//    //    0,
//    //    nullptr
//    //);
//
//    //// Specify the buffers we are going to render to.
//    //// 指定将要渲染的缓冲区
//    //m_command_list->OMSetRenderTargets
//    //(
//    //    1, 
//    //    &CurrentBackBufferView(),
//    //    true,
//    //    &DepthStencilView()
//    //);
//
//    //// Indicate a state transition on the resource usage.
//    //// 再次对资源状态进行转换，将资源从渲染目标状态转换回呈现状态
//    //m_command_list->ResourceBarrier
//    //(
//    //    1, 
//    //    &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
//    //    D3D12_RESOURCE_STATE_RENDER_TARGET, 
//    //    D3D12_RESOURCE_STATE_PRESENT)
//    //);
//
//    //// Done recording commands.
//    //// 完成命令记录
//    //ThrowIfFailed(m_command_list->Close());
//
//    //// Add the command list to the queue for execution.
//    //// 将待执行的命令列表加入命令队列
//    //ID3D12CommandList* command_lists[] = { m_command_list.Get() };
//    //m_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);
//
//    UpdateBox();
//    DrawBox();
//
//    return true;
//}
//
////--------------------------------------------------------------------------------
////  End render and present the buffers 
////  描画終了、バッファの切り替え
////  渲染完成并交换画面缓存
////--------------------------------------------------------------------------------
//void RenderSystemDirectX12::EndRender()
//{
//    //// swap the back and front buffers
//    //// 交换前台与后台缓冲区
//    //ThrowIfFailed(m_swap_chain->Present(0, 0));
//    //m_current_back_buffer = (m_current_back_buffer + 1) % sc_swap_chain_buffer_count;
//
//    //// Wait until frame commands are complete.  This waiting is inefficient and is
//    //// done for simplicity.  Later we will show how to organize our rendering code
//    //// so we do not have to wait per frame.
//    //// 等待此前的命令执行完毕，当前的实现没有什么效果，也过于简单
//    //// 在后面将重新组织渲染部分的代码，以免在每一帧都要等待
//    //FlushCommandQueue();
//}

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
    for (uint32_t i = 0; i < sc_swap_chain_buffer_count; i++)
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
void RenderSystemDirectX12::CreateDepthStencilBufferView(const uint32_t width, const uint32_t height)
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
//  Create the frame resources
//  frame resourcesの作成
//  创建帧资源
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::CreateFrameResources()
{
    for (uint32_t i = 0; i < sc_num_frame_resources; ++i)
    {
        m_frame_resources[i] = MY_NEW FrameResource(m_device.Get(), 1, static_cast<uint32_t>(mAllRitems.size()));
    }
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

//--------------------------------------------------------------------------------
//  update FrameResource
//  FrameResource更新
//  更新帧资源
//--------------------------------------------------------------------------------
void RenderSystemDirectX12::UpdateFrameResource()
{
    {// Current
        // 增加围栏值,将命令标记到此围栏点
        // Advance the fence value to mark commands up to this fence point.
        m_frame_resources[m_current_frame_resource_index]->SetCurrentFence(++m_current_fence);

        // 向命令队列添加一条指令来设置一个新的围栏点
        // 由于当前的gpu正在执行绘制命令,所以在gpu处理完signal()函数之前的所有命令以前
        // 并不会设置此新的围栏点
        // Add an instruction to the command queue to set a new fence point. 
        // Because we are on the GPU timeline, the new fence point won't be 
        // set until the GPU finishes processing all the commands prior to this Signal().
        m_command_queue->Signal(m_fence.Get(), m_current_fence);

        // gpu此时可能仍然在处理上一帧数据,但是这也没什么问题,因为我们这些操作并没有
        // 影响与之前帧相关联的帧资源
    }

    {// Next
        // 循环获取帧资源循环数组中的元素
        // Cycle through the circular frame resource array.
        m_current_frame_resource_index = (m_current_frame_resource_index + 1) % sc_num_frame_resources;

        // gpu端是否已经执行完处理当前帧资源的所有命令了呢?
        // 如果还没有就令cpu等待, 直到gpu执行完成并抵达这个围栏点
        // Has the GPU finished processing the commands of the current frame resource?
        // If not, wait until the GPU has completed commands up to this fence point.
        auto current_fence = m_frame_resources[m_current_frame_resource_index]->CurrentFence();
        if (current_fence != 0
            && m_fence->GetCompletedValue() < current_fence)
        {
            HANDLE event_handle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
            ThrowIfFailed(m_fence->SetEventOnCompletion(current_fence, event_handle));
            WaitForSingleObject(event_handle, INFINITE);
            CloseHandle(event_handle);
        }
    }
}

//Test
void RenderSystemDirectX12::InitTest()
{
    // Reset the command list to prep for initialization commands.
    ThrowIfFailed(m_command_list->Reset(m_command_list_allocator.Get(), nullptr));

    BuildRootSignature();
    BuildGeometry();
    BuildRenderItems();
    CreateFrameResources();
    BuildDescriptorHeaps();
    BuildConstantBufferViews();
    BuildPSOs();

    // Execute the initialization commands.
    ThrowIfFailed(m_command_list->Close());
    ID3D12CommandList* command_lists[] = { m_command_list.Get() };
    m_command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

    // Wait until initialization is complete.
    FlushCommandQueue();
}

void RenderSystemDirectX12::UninitTest()
{
    for (auto item : mAllRitems)
    {
        SAFE_DELETE(item);
    }

    for (auto geo : mGeometries)
    {
        SAFE_DELETE(geo.second);
    }
}

void RenderSystemDirectX12::UpdateTest()
{
    UpdateCamera();
    UpdateObjectCBs();
    UpdateMainPassCB();
}

void RenderSystemDirectX12::UpdateCamera()
{
    static float mTheta = 0.0f;
    mTheta += XM_PI * GameTimer::Instance().DeltaTime() * 0.1f;
    float mPhi = XM_PIDIV4 * 1.5f;
    float mRadius = 10.0f;

    // Convert Spherical to Cartesian coordinates.
    mEyePos.x = mRadius * sinf(mPhi) * cosf(mTheta);
    mEyePos.z = mRadius * sinf(mPhi) * sinf(mTheta);
    mEyePos.y = mRadius * cosf(mPhi);

    // Build the view matrix.
    XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    mView = XMMatrixLookAtLH(pos, target, up);

    float width = static_cast<float>(MainSystem::Instance().Width());
    float height = static_cast<float>(MainSystem::Instance().Height());
    mProj = XMMatrixPerspectiveFovLH(0.25f * XM_PI, width / height, 1.0f, 1000.0f);
}

void RenderSystemDirectX12::UpdateObjectCBs()
{
    auto currObjectCB = m_frame_resources[m_current_frame_resource_index]->ObjectCbuffer();
    for (auto& e : mAllRitems)
    {
        // 只要常量发生了改变就得更新常量缓冲区内的数据,而且要对每个帧资源都进行更新
        // Only update the cbuffer data if the constants have changed.  
        // This needs to be tracked per frame resource.
        if (e->NumFramesDirty > 0)
        {
            ObjectConstants objConstants;
            objConstants.world = XMMatrixTranspose(e->World);

            currObjectCB->CopyData(e->ObjCBIndex, objConstants);

            // 还需要对下一个frameresource进行更新
            // Next FrameResource need to be updated too.
            e->NumFramesDirty--;
        }
    }
}

void RenderSystemDirectX12::UpdateMainPassCB()
{
    XMMATRIX viewProj = XMMatrixMultiply(mView, mProj);
    XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(mView), mView);
    XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(mProj), mProj);
    XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    mMainPassCB.View = XMMatrixTranspose(mView);
    mMainPassCB.InvView = XMMatrixTranspose(invView);
    mMainPassCB.Proj = XMMatrixTranspose(mView);
    mMainPassCB.InvProj = XMMatrixTranspose(invProj);
    mMainPassCB.ViewProj = XMMatrixTranspose(viewProj);
    mMainPassCB.InvViewProj = XMMatrixTranspose(invViewProj);
    mMainPassCB.EyePosW = mEyePos;
    mMainPassCB.RenderTargetSize = XMFLOAT2((float)MainSystem::Instance().Width(), (float)MainSystem::Instance().Height());
    mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / MainSystem::Instance().Width(), 1.0f / MainSystem::Instance().Height());
    mMainPassCB.NearZ = 1.0f;
    mMainPassCB.FarZ = 1000.0f;
    mMainPassCB.TotalTime = 0.0f;
    mMainPassCB.DeltaTime = GameTimer::Instance().DeltaTime();

    auto currPassCB = m_frame_resources[m_current_frame_resource_index]->PassCbuffer();
    currPassCB->CopyData(0, mMainPassCB);
}

void RenderSystemDirectX12::DrawTest()
{
    auto cmdListAlloc = m_frame_resources[m_current_frame_resource_index]->CommandListAllocator();

    // 复用与记录命令有关的内存
    // 只有在gpu执行完与该内存相关联的命令列表时,才能对此命令列表分配器进行重置
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    ThrowIfFailed(cmdListAlloc->Reset());

    // 在通过ExecuteCommandList方法将命令列表添加到命令队列中之后,
    // 就可以对他进行重置,复用命令列表即复用与之相关的内存
    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    ThrowIfFailed(m_command_list->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
    //if (mIsWireframe)
    //{
    //    ThrowIfFailed(m_command_list->Reset(cmdListAlloc.Get(), mPSOs["opaque_wireframe"].Get()));
    //}
    //else
    //{
    //    ThrowIfFailed(m_command_list->Reset(cmdListAlloc.Get(), mPSOs["opaque"].Get()));
    //}

    m_command_list->RSSetViewports(1, &m_screen_viewport);
    m_command_list->RSSetScissorRects(1, &m_scissor_rect);

    // 根据资源的用途指示资源状态的转换
    // Indicate a state transition on the resource usage.
    m_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // 清除后台缓冲区和深度缓冲区
    // Clear the back buffer and depth buffer.
    m_command_list->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    m_command_list->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // 指定要渲染的目标缓冲区
    // Specify the buffers we are going to render to.
    m_command_list->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    /////////////////////////////////////////////////////////////////////////////
    // 先将根签名和cbv堆设置到命令列表上，并随后再通过设置描述符表来制定我们
    // 希望绑定到渲染流水线的资源
    ID3D12DescriptorHeap* descriptor_heaps[] = { mCbvHeap.Get() };
    m_command_list->SetDescriptorHeaps(_countof(descriptor_heaps), descriptor_heaps);
    
    m_command_list->SetGraphicsRootSignature(mRootSignature.Get());
    
    int passCbvIndex = mPassCbvOffset + m_current_frame_resource_index;
    auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
    passCbvHandle.Offset(passCbvIndex, m_cbv_srv_uav_descriptor_size);
    m_command_list->SetGraphicsRootDescriptorTable(
        1,// 将跟参数按此索引（即与绑定到的寄存器槽号）进行设置
        // 此参数指定的是将要想着色器绑定的描述符表中第一个描述符位于描述符
        // 堆中的句柄。比如说，如果根签名知名当前描述符表中共有5个描述符，
        // 则堆中的BaseDescriptor及其后面的4个描述符将被设置到此描述符表中
        passCbvHandle);
    //
    /////////////////////////////////////////////////////////////////////////////

    DrawRenderItems(mOpaqueRitems);

    // 按照资源的用途指示资源状态的转换
    // Indicate a state transition on the resource usage.
    m_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    // 完成命令的记录
    // Done recording commands.
    ThrowIfFailed(m_command_list->Close());

    // 将命令列表加入到命令队列中用于执行
    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = { m_command_list.Get() };
    m_command_queue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // 交换前后台缓冲区
    // Swap the back and front buffers
    ThrowIfFailed(m_swap_chain->Present(0, 0));
    m_current_back_buffer = (m_current_back_buffer + 1) % sc_swap_chain_buffer_count;
}

void RenderSystemDirectX12::DrawRenderItems(const std::vector<RenderItem*>& ritems)
{
    uint32_t objCBByteSize = Utility::CalculateConstantBufferByteSize(sizeof(ObjectConstants));

    auto objectCB = m_frame_resources[m_current_frame_resource_index]->ObjectCbuffer()->Resource();

    // 对于每个渲染项来说
    // For each render item...
    for (size_t i = 0; i < ritems.size(); ++i)
    {
        auto ri = ritems[i];

        m_command_list->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
        m_command_list->IASetIndexBuffer(&ri->Geo->IndexBufferView());
        m_command_list->IASetPrimitiveTopology(ri->PrimitiveType);

        // 为了绘制当前的帧资源和当前物体,偏移到描述符堆中对应的cbv处
        // Offset to the CBV in the descriptor heap for this object and for this frame resource.
        uint32_t cbvIndex = m_current_frame_resource_index * (uint32_t)mOpaqueRitems.size() + ri->ObjCBIndex;
        auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
        cbvHandle.Offset(cbvIndex, m_cbv_srv_uav_descriptor_size);

        m_command_list->SetGraphicsRootDescriptorTable(0, cbvHandle);

        m_command_list->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
    }
}

// 利用描述符将常量缓冲区绑定到渲染流水线上
void RenderSystemDirectX12::BuildDescriptorHeaps()
{
    // 如果有3个帧资源与n个渲染项,那么就应存在3n个物体常量缓冲区(object constant buffer)
    // 以及3个渲染过程常量缓冲区(pass constant buffer)
    // 因此我们也就需要创建3(n+1)个常量缓冲区视图(cbv)
    uint32_t objCount = (uint32_t)mOpaqueRitems.size();

    // 我们需要为每个帧资源中的每一个物体都创建一个cbv描述符
    // 为了容纳每个帧资源中的渲染过程cbv而+1
    // Need a CBV descriptor for each object for each frame resource,
    // +1 for the perPass CBV for each frame resource.
    uint32_t numDescriptors = (objCount + 1) * sc_num_frame_resources;

    // 保存渲染过程cbv的起始偏移量,在本程序中,这是排在最后面的3个描述符
    // Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
    mPassCbvOffset = objCount * sc_num_frame_resources;

    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
    cbvHeapDesc.NumDescriptors = numDescriptors;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&mCbvHeap)));
}

void RenderSystemDirectX12::BuildConstantBufferViews()
{
    uint32_t objCBByteSize = Utility::CalculateConstantBufferByteSize(sizeof(ObjectConstants));

    uint32_t objCount = (uint32_t)mOpaqueRitems.size();

    // 每个帧资源中的每一个物体都需要一个对应的cbv描述符
    // Need a CBV descriptor for each object for each frame resource.
    for (int frameIndex = 0; frameIndex < sc_num_frame_resources; ++frameIndex)
    {
        auto objectCB = m_frame_resources[frameIndex]->ObjectCbuffer()->Resource();

        for (uint32_t i = 0; i < objCount; ++i)
        {
            // 缓冲区的起始地址（即索引为0的那个常量缓冲区的地址）
            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

            // 偏移到常量缓冲区中绘制第i个物体所需的常量数据
            // Offset to the ith object constant buffer in the buffer.
            cbAddress += i * objCBByteSize;

            // 偏移到该物体在描述符队中的cbv
            // Offset to the object cbv in the descriptor heap.
            int heapIndex = frameIndex * objCount + i;
            auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
            handle.Offset(heapIndex, m_cbv_srv_uav_descriptor_size);

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = objCBByteSize;

            m_device->CreateConstantBufferView(&cbvDesc, handle);
        }
    }

    uint32_t passCBByteSize = Utility::CalculateConstantBufferByteSize(sizeof(PassConstants));

    // 最后3个描述符依次是每个帧资源的渲染过程cbv
    // Last three descriptors are the pass CBVs for each frame resource.
    for (int frameIndex = 0; frameIndex < sc_num_frame_resources; ++frameIndex)
    {
        auto passCB = m_frame_resources[frameIndex]->PassCbuffer()->Resource();

        // 每个帧资源的渲染过程缓冲区中只有一个常量缓冲区
        D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

        // 偏移到描述符堆中对应的渲染过程cbv
        // Offset to the pass cbv in the descriptor heap.
        int heapIndex = mPassCbvOffset + frameIndex;
        auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
        handle.Offset(heapIndex, m_cbv_srv_uav_descriptor_size);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        cbvDesc.BufferLocation = cbAddress;
        cbvDesc.SizeInBytes = passCBByteSize;

        m_device->CreateConstantBufferView(&cbvDesc, handle);
    }
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

    CD3DX12_DESCRIPTOR_RANGE cbvTable0;
    cbvTable0.Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_CBV,// 描述符表的类型
        1,                              // 表中的描述符数量
        0);                             // 将这段描述符区域绑定至此基准着色器寄存器（base shader register）

    CD3DX12_DESCRIPTOR_RANGE cbvTable1;
    cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[2] = {};

    // Create root CBVs.
    slotRootParameter[0].InitAsDescriptorTable(
        1,          // 描述符区域的数量
        &cbvTable0);// 只想描述符区域数组的指针
    slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // 创建仅含一个槽位（该槽位指向一个仅由单个常量缓冲区组成的描述符区域）的根签名
    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    ThrowIfFailed(m_device->CreateRootSignature(
        0,
        serializedRootSig->GetBufferPointer(),
        serializedRootSig->GetBufferSize(),
        IID_PPV_ARGS(mRootSignature.GetAddressOf())));
}

void RenderSystemDirectX12::BuildGeometry()
{
    MeshData box = MeshGenerator::CreateBox(1.5f, 0.5f, 1.5f, 3);
    MeshData grid = MeshGenerator::CreateGrid(20.0f, 30.0f, 60, 40);
    MeshData sphere = MeshGenerator::CreateSphere(0.5f, 20, 20);
    MeshData cylinder = MeshGenerator::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

    // 
    // 将所有的几何体数据都合并到一对大的顶点索引缓冲区中
    // 以此来定义每个子网格数据在缓冲区中所占的范围
    // We are concatenating all the geometry into one big vertex/index buffer.  So
    // define the regions in the buffer each submesh covers.
    //

    // 对合并顶点缓冲区中每个物体的顶点偏移量进行缓存
    // Cache the vertex offsets to each object in the concatenated vertex buffer.
    uint32_t boxVertexOffset = 0;
    uint32_t gridVertexOffset = (uint32_t)box.vertices.size();
    uint32_t sphereVertexOffset = gridVertexOffset + (uint32_t)grid.vertices.size();
    uint32_t cylinderVertexOffset = sphereVertexOffset + (uint32_t)sphere.vertices.size();

    // 对合并索引缓冲区中每个物体的起始索引进行缓存
    // Cache the starting index for each object in the concatenated index buffer.
    uint32_t boxIndexOffset = 0;
    uint32_t gridIndexOffset = (uint32_t)box.indeces.size();
    uint32_t sphereIndexOffset = gridIndexOffset + (uint32_t)grid.indeces.size();
    uint32_t cylinderIndexOffset = sphereIndexOffset + (uint32_t)sphere.indeces.size();

    // 定义的多个UnitMeshGeometry结构体中包含了顶点索引缓冲区内不同几何体的子网格数据
    // Define the UnitMeshGeometry that cover different 
    // regions of the vertex/index buffers.

    UnitMeshGeometry boxSubmesh;
    boxSubmesh.index_count = (uint32_t)box.indeces.size();
    boxSubmesh.start_index_location = boxIndexOffset;
    boxSubmesh.base_vertex_location = boxVertexOffset;

    UnitMeshGeometry gridSubmesh;
    gridSubmesh.index_count = (uint32_t)grid.indeces.size();
    gridSubmesh.start_index_location = gridIndexOffset;
    gridSubmesh.base_vertex_location = gridVertexOffset;

    UnitMeshGeometry sphereSubmesh;
    sphereSubmesh.index_count = (uint32_t)sphere.indeces.size();
    sphereSubmesh.start_index_location = sphereIndexOffset;
    sphereSubmesh.base_vertex_location = sphereVertexOffset;

    UnitMeshGeometry cylinderSubmesh;
    cylinderSubmesh.index_count = (uint32_t)cylinder.indeces.size();
    cylinderSubmesh.start_index_location = cylinderIndexOffset;
    cylinderSubmesh.base_vertex_location = cylinderVertexOffset;

    // 
    // 提取出所需的顶点元素,再将所有网格的顶点装进一个顶点缓冲区
    // Extract the vertex elements we are interested in and pack the
    // vertices of all the meshes into one vertex buffer.
    //

    auto totalVertexCount =
        box.vertices.size() +
        grid.vertices.size() +
        sphere.vertices.size() +
        cylinder.vertices.size();

    std::vector<VertexTest> vertices(totalVertexCount);

    uint32_t k = 0;
    for (size_t i = 0; i < box.vertices.size(); ++i, ++k)
    {
        vertices[k].position = box.vertices[i].position;
        vertices[k].color = XMFLOAT4(DirectX::Colors::DarkGreen);
    }

    for (size_t i = 0; i < grid.vertices.size(); ++i, ++k)
    {
        vertices[k].position = grid.vertices[i].position;
        vertices[k].color = XMFLOAT4(DirectX::Colors::ForestGreen);
    }

    for (size_t i = 0; i < sphere.vertices.size(); ++i, ++k)
    {
        vertices[k].position = sphere.vertices[i].position;
        vertices[k].color = XMFLOAT4(DirectX::Colors::Crimson);
    }

    for (size_t i = 0; i < cylinder.vertices.size(); ++i, ++k)
    {
        vertices[k].position = cylinder.vertices[i].position;
        vertices[k].color = XMFLOAT4(DirectX::Colors::SteelBlue);
    }

    std::vector<uint32_t> indices;
    indices.insert(indices.end(), std::begin(box.indeces), std::end(box.indeces));
    indices.insert(indices.end(), std::begin(grid.indeces), std::end(grid.indeces));
    indices.insert(indices.end(), std::begin(sphere.indeces), std::end(sphere.indeces));
    indices.insert(indices.end(), std::begin(cylinder.indeces), std::end(cylinder.indeces));

    const uint32_t vbByteSize = (uint32_t)vertices.size() * sizeof(VertexTest);
    const uint32_t ibByteSize = (uint32_t)indices.size() * sizeof(uint32_t);

    auto geo = new MeshGeometry();
    geo->name = "shapeGeo";

    ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->vertex_buffer_cpu));
    CopyMemory(geo->vertex_buffer_cpu->GetBufferPointer(), vertices.data(), vbByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->index_buffer_cpu));
    CopyMemory(geo->index_buffer_cpu->GetBufferPointer(), indices.data(), ibByteSize);

    geo->vertex_buffer_gpu = CreateDefaultBuffer(vertices.data(), vbByteSize, geo->vertex_buffer_uploader);

    geo->index_buffer_gpu = CreateDefaultBuffer(indices.data(), ibByteSize, geo->index_buffer_uploader);

    geo->vertex_byte_stride = sizeof(VertexTest);
    geo->vertex_buffer_byte_size = vbByteSize;
    geo->index_format = DXGI_FORMAT_R32_UINT;
    geo->index_buffer_byte_size = ibByteSize;

    geo->unit_mesh_geometries["box"] = boxSubmesh;
    geo->unit_mesh_geometries["grid"] = gridSubmesh;
    geo->unit_mesh_geometries["sphere"] = sphereSubmesh;
    geo->unit_mesh_geometries["cylinder"] = cylinderSubmesh;

    mGeometries[geo->name] = geo;
}

void RenderSystemDirectX12::BuildPSOs()
{
    // BuildShadersAndInputLayout
    mShaders["standardVS"] = CompileShader(L"SourceCodes\\Systems\\RenderSystem\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["opaquePS"] = CompileShader(L"SourceCodes\\Systems\\RenderSystem\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");

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
    psoDesc.pRootSignature = mRootSignature.Get(); // 指向一个与此pso像绑定的根签名指针。该根签名一定要于此PSO指定的着色器相兼容
    psoDesc.InputLayout = { mInputLayout.data(), (uint32_t)mInputLayout.size() }; // 输入布局描述
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // 指定用来配置光栅器的光栅化状态
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // 指定混合操作所用的混合状态
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // 指定用于配置深度/模板状态。
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;  // 指定图元的拓扑类型
    psoDesc.NumRenderTargets = 1; // 同时所用的渲染目标数量（即RTVFormats数组中渲染目标格式的数量）
    psoDesc.RTVFormats[0] = m_back_buffer_format; // 渲染目标的格式。利用该数组实现向多渲染目标同时进行写操作
    psoDesc.DSVFormat = m_depth_stencil_format; // 深度/模板缓冲区的格式

    // 待绑定的顶点着色器，由结构体D3D12_SHADER_BYTECODE表示
    // 这个结构体存有指向已编译好的字节码数据的指针
    // 以及该字节码数据所占的字节大小
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
        mShaders["standardVS"]->GetBufferSize()
    };

    // 待绑定的像素着色器
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
        mShaders["opaquePS"]->GetBufferSize()
    };
     
    // 多重采样最多可采集32个样本。借此参数的32位整数值，即可设置每个采样点的采集情况
    // （采集或禁止采集）。例如若禁用了第五位（将第五位设置为0），则将不会对第五个样本
    // 进行采样。当然，要禁止采集第五个样本的前提是，所用的多重采样至少要有五个样本。
    // 假如一个应用程序仅使用了单采样（single sampling），那么只能针对该参数的第一位
    // 进行配置。一般来说，使用的都是默认值0xffffffff，即表示对所有的采样点都进行采样
    psoDesc.SampleMask = UINT_MAX;

    // 描述多重采样对每个像素采样的数量及其质量级别
    psoDesc.SampleDesc.Count = m_msaa_enable ? 4 : 1;
    psoDesc.SampleDesc.Quality = m_msaa_enable ? (m_msaa_quality - 1) : 0;
    //
    //--------------------------------------------------------------------------------

    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

    //
    // PSO for opaque wireframe objects.
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = psoDesc;
    opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
}

void RenderSystemDirectX12::BuildRenderItems()
{
    auto boxRitem = MY_NEW RenderItem();
    boxRitem->World = XMMatrixScaling(2.0f, 2.0f, 2.0f) * XMMatrixTranslation(0.0f, 0.5f, 0.0f);
    boxRitem->ObjCBIndex = 0;
    boxRitem->Geo = mGeometries["shapeGeo"];
    boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    boxRitem->IndexCount = boxRitem->Geo->unit_mesh_geometries["box"].index_count;
    boxRitem->StartIndexLocation = boxRitem->Geo->unit_mesh_geometries["box"].start_index_location;
    boxRitem->BaseVertexLocation = boxRitem->Geo->unit_mesh_geometries["box"].base_vertex_location;
    mAllRitems.push_back(boxRitem);

    auto gridRitem = MY_NEW RenderItem();
    gridRitem->World = XMMatrixIdentity();
    gridRitem->ObjCBIndex = 1;
    gridRitem->Geo = mGeometries["shapeGeo"];
    gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    gridRitem->IndexCount = gridRitem->Geo->unit_mesh_geometries["grid"].index_count;
    gridRitem->StartIndexLocation = gridRitem->Geo->unit_mesh_geometries["grid"].start_index_location;
    gridRitem->BaseVertexLocation = gridRitem->Geo->unit_mesh_geometries["grid"].base_vertex_location;
    mAllRitems.push_back(gridRitem);

    uint32_t objCBIndex = 2;
    for (int i = 0; i < 5; ++i)
    {
        auto leftCylRitem = MY_NEW RenderItem();
        auto rightCylRitem = MY_NEW RenderItem();
        auto leftSphereRitem = MY_NEW RenderItem();
        auto rightSphereRitem = MY_NEW RenderItem();

        XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
        XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

        XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
        XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

        leftCylRitem->World = rightCylWorld;
        leftCylRitem->ObjCBIndex = objCBIndex++;
        leftCylRitem->Geo = mGeometries["shapeGeo"];
        leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        leftCylRitem->IndexCount = leftCylRitem->Geo->unit_mesh_geometries["cylinder"].index_count;
        leftCylRitem->StartIndexLocation = leftCylRitem->Geo->unit_mesh_geometries["cylinder"].start_index_location;
        leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->unit_mesh_geometries["cylinder"].base_vertex_location;

        rightCylRitem->World = leftCylWorld;
        rightCylRitem->ObjCBIndex = objCBIndex++;
        rightCylRitem->Geo = mGeometries["shapeGeo"];
        rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        rightCylRitem->IndexCount = rightCylRitem->Geo->unit_mesh_geometries["cylinder"].index_count;
        rightCylRitem->StartIndexLocation = rightCylRitem->Geo->unit_mesh_geometries["cylinder"].start_index_location;
        rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->unit_mesh_geometries["cylinder"].base_vertex_location;

        leftSphereRitem->World = leftSphereWorld;
        leftSphereRitem->ObjCBIndex = objCBIndex++;
        leftSphereRitem->Geo = mGeometries["shapeGeo"];
        leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        leftSphereRitem->IndexCount = leftSphereRitem->Geo->unit_mesh_geometries["sphere"].index_count;
        leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->unit_mesh_geometries["sphere"].start_index_location;
        leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->unit_mesh_geometries["sphere"].base_vertex_location;

        rightSphereRitem->World = rightSphereWorld;
        rightSphereRitem->ObjCBIndex = objCBIndex++;
        rightSphereRitem->Geo = mGeometries["shapeGeo"];
        rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        rightSphereRitem->IndexCount = rightSphereRitem->Geo->unit_mesh_geometries["sphere"].index_count;
        rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->unit_mesh_geometries["sphere"].start_index_location;
        rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->unit_mesh_geometries["sphere"].base_vertex_location;

        mAllRitems.push_back(leftCylRitem);
        mAllRitems.push_back(rightCylRitem);
        mAllRitems.push_back(leftSphereRitem);
        mAllRitems.push_back(rightSphereRitem);
    }

    // All the render items are opaque.
    for (auto& e : mAllRitems)
        mOpaqueRitems.push_back(e);
}

// 利用作为中介的上传缓冲区来初始化默认缓冲区
ComPtr<ID3D12Resource> RenderSystemDirectX12::CreateDefaultBuffer(const void* init_data, uint64_t byte_size, ComPtr<ID3D12Resource>& upload_buffer)
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
    uint32_t compile_flags = 0;
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