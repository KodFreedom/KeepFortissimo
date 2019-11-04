//--------------------------------------------------------------------------------
//  base class of render system
//  描画システムのベース
//  渲染系统的基类
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "render_system.h"
#include "render_system_directx12.h"
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
bool RenderSystem::StartUp(const RenderApiType type)
{
    if (m_instance != nullptr) return true;

    switch (type)
    {
    case RenderApiType::kDirectX12:
        MY_NEW RenderSystemDirectX12();
        break;
    default:
        MY_NEW RenderSystem(RenderApiType::kInvalid);
        break;
    }

    return m_instance->Initialize();
}

//--------------------------------------------------------------------------------
//  SetMsaaEnable
//--------------------------------------------------------------------------------
void RenderSystem::SetMsaaEnable(bool value)
{
    m_msaa_enable = value;
}

//--------------------------------------------------------------------------------
//
//  Protected
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  RenderSystem
//--------------------------------------------------------------------------------
RenderSystem::RenderSystem(const RenderApiType type)
    : Singleton<RenderSystem>()
    , m_api_type(type)
{
}

//--------------------------------------------------------------------------------
//  ~RenderSystem
//--------------------------------------------------------------------------------
RenderSystem::~RenderSystem()
{
}