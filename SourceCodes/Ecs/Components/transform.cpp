//--------------------------------------------------------------------------------
//  transform component
//  トランスフォ�`ムコンポネント
//  了崔秤烏怏周
//
//  Autor  : 俾 猟旬(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "transform.h"
#include "../entity.h"
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//  constructor
//  コンストラクタ
//  更夛痕方
//--------------------------------------------------------------------------------
Transform::Transform(Entity& owner)
    : Component(owner)
{
    
}

//--------------------------------------------------------------------------------
//  calculate the world matrix
//  ワ�`ルドマトリックスの麻竃
//  柴麻弊順裳專
//--------------------------------------------------------------------------------
void Transform::CalculateWorld()
{
    m_world = DirectX::XMMatrixScalingFromVector(m_scalling);
    m_world *= DirectX::XMMatrixRotationQuaternion(m_rotation);
    m_world *= DirectX::XMMatrixTranslationFromVector(m_position);

    if (Entity* parent = GetOwner().GetParent())
    {
        m_world *= parent->GetTransform().GetWorld();
    }

    for (auto pair : GetOwner().GetChildren())
    {
        pair.second->GetTransform().CalculateWorld();
    }
}