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
#include "../../Ecs/Components/Renderer/renderer.h"
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
//  Render all registered components
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  登録したコンポネントを描画する
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  将注册过的组件进行渲染
//--------------------------------------------------------------------------------
 void RenderSystem::Render()
{
     CleanUp();
}

//--------------------------------------------------------------------------------
//  SetMsaaEnable
//--------------------------------------------------------------------------------
void RenderSystem::SetMsaaEnable(bool value)
{
    m_msaa_enable = value;
}

//--------------------------------------------------------------------------------
//  Add renderer
//--------------------------------------------------------------------------------
void RenderSystem::Add(Renderer* renderer)
{
    if (!renderer) return;
    m_renderers[static_cast<uint32_t>(renderer->GetRenderPriority())].push_back(renderer);
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

//--------------------------------------------------------------------------------
//  Clean up
//  クリーンアップ
//  善后
//--------------------------------------------------------------------------------
void RenderSystem::CleanUp()
{
    for (std::list<Renderer*>& renderers : m_renderers)
    {
        renderers.clear();
    }
}