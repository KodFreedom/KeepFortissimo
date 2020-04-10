//--------------------------------------------------------------------------------
//  renderer component
//  ������`����ݥͥ��
//  �軭���
//
//  Autor  : �� �Ľ�(Wenjie Xu)
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
//  ���󥹥ȥ饯��
//  ���캯��
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