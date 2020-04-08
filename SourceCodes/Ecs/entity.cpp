//--------------------------------------------------------------------------------
//  entity class
//  ����ƥ��ƥ����饹
//  ʵ����
//
//  Autor  : �� �Ľ�(Wenjie Xu)
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
//  �H���h�I��
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
//  �ӹ����h�I��
//--------------------------------------------------------------------------------
void Entity::AddChild(Entity* child)
{
    if (!child) return;
    m_children.emplace(child->GetId(), child);
}

//--------------------------------------------------------------------------------
//  �ӹ������I��
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
//  ���󥹥ȥ饯��
//  ���캯��
//--------------------------------------------------------------------------------
Entity::Entity(const uint32_t id)
    : m_id(id)
{

}

//--------------------------------------------------------------------------------
//  destructor
//  �ǥ��ȥ饯��
//  ��������
//--------------------------------------------------------------------------------
Entity::~Entity()
{

}

//--------------------------------------------------------------------------------
//  �K�˄I��
//  ��ֹ
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