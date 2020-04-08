//--------------------------------------------------------------------------------
//  entity system
//  ����ƥ��ƥ������ƥ�
//  ʵ��ϵͳ
//
//  Autor  : �� �Ľ�(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "entity_system.h"
#include "entity.h"
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//  ����ƥ��ƥ�����
//  ����ʵ��
//--------------------------------------------------------------------------------
Entity* EntitySystem::CreateEntity(Entity* parent)
{
    Entity* entity = MY_NEW Entity(m_entity_id_end);
    entity->Initialize();
    entity->SetParent(parent);
    ++m_entity_id_end;
    return entity;
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//  Uninit the instance
//  ���󥹥��󥹤νK�˄I��
//  ���˴���
//--------------------------------------------------------------------------------
void EntitySystem::Uninitialize()
{
    for (auto pair : m_root_entities)
    {
        SAFE_UNINIT(pair.second);
    }
}

//--------------------------------------------------------------------------------
//  ��`�ȥ���ƥ��ƥ�׷��
//  ׷�Ӹ�ʵ��
//--------------------------------------------------------------------------------
void EntitySystem::AddRootEntity(Entity* entity)
{
    if (!entity) return;

    auto iterator = m_root_entities.find(entity->GetId());
    if (iterator == m_root_entities.end())
    {
        m_root_entities.emplace(entity->GetId(), entity);
    }
}

//--------------------------------------------------------------------------------
//  ��`�ȥ���ƥ��ƥ�����������ƥ��ƥ���������������ޤ���
//  �Ƴ���ʵ��
//--------------------------------------------------------------------------------
void EntitySystem::RemoveRootEntity(Entity* entity)
{
    if (!entity) return;

    auto iterator = m_root_entities.find(entity->GetId());
    if (iterator != m_root_entities.end())
    {
        m_root_entities.erase(iterator);
    }
}
