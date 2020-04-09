//--------------------------------------------------------------------------------
//  entity class
//  エンティティクラス
//  实体类
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "../Systems/system_setting.h"
#include "entity.h"
#include "entity_system.h"
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//  親登録処理
//--------------------------------------------------------------------------------
void Entity::SetParent(Entity* parent)
{
    if (m_parent)
    {
        m_parent->RemoveChild(this);
    }
    else
    {
        EntitySystem::Instance().RemoveRootEntity(this);
    }

    m_parent = parent;

    if (m_parent)
    {
        m_parent->AddChild(this);
    }
    else
    {
        EntitySystem::Instance().AddRootEntity(this);
    }
}

//--------------------------------------------------------------------------------
//  子供登録処理
//--------------------------------------------------------------------------------
void Entity::AddChild(Entity* child)
{
    if (!child) return;
    m_children.emplace(child->GetId(), child);
}

//--------------------------------------------------------------------------------
//  子供削除処理
//--------------------------------------------------------------------------------
void Entity::RemoveChild(Entity* child)
{
    if (!child) return;
    auto iterator = m_children.find(child->GetId());
    if (iterator != m_children.end())
    {
        m_children.erase(iterator);
    }
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//  constructor
//  コンストラクタ
//  构造函数
//--------------------------------------------------------------------------------
Entity::Entity(const uint32_t id)
    : m_id(id)
{

}

//--------------------------------------------------------------------------------
//  destructor
//  デストラクタ
//  析构函数
//--------------------------------------------------------------------------------
Entity::~Entity()
{

}

//--------------------------------------------------------------------------------
//  終了処理
//  终止
//--------------------------------------------------------------------------------
void Entity::Uninitialize()
{
    for (auto pair : m_children)
    {
        SAFE_UNINIT(pair.second);
    }

    for (auto pair : m_components)
    {
        SAFE_UNINIT(pair.second);
    }
}