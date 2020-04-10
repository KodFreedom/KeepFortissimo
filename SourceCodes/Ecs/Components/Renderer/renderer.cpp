//--------------------------------------------------------------------------------
//  renderer component
//  レンダラーコンポネント
//  描画组件
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "renderer.h"
#include "../../entity.h"
#include "../transform.h"
#include "../../../Systems/RenderSystem/render_define.h"
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//  constructor
//  コンストラクタ
//  构造函数
//--------------------------------------------------------------------------------
Renderer::Renderer(Entity& owner) 
    : Component(owner)
    , m_render_priority(RenderPriority::kOpaque)
{

}

//--------------------------------------------------------------------------------
//  getter
//--------------------------------------------------------------------------------
const DirectX::XMMATRIX& Renderer::GetWorld()
{
    return GetOwner().GetTransform().GetWorld();
}