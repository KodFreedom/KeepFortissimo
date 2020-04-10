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
#include "Components/transform.h"
#include "Components/Renderer/renderer.h"
#include "../Systems/RenderSystem/render_system.h"
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

// test
void EntitySystem::PrepareRender()
{
    for (auto pair : m_root_entities)
    {
        pair.second->GetTransform().CalculateWorld();
    }

#if _DEBUG
    for (auto pair : m_entities)
    {
        Renderer* renderer = pair.second->GetComponent<Renderer>();
        if (renderer)
        {
            RenderSystem::Instance().Add(renderer);
        }
    }
#endif
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

#if _DEBUG
//--------------------------------------------------------------------------------
//  ����ƥ��ƥ�׷��
//  ׷��ʵ��
//--------------------------------------------------------------------------------
void EntitySystem::AddEntity(Entity* entity)
{
    if (!entity) return;

    auto iterator = m_entities.find(entity->GetId());
    if (iterator == m_entities.end())
    {
        m_entities.emplace(entity->GetId(), entity);
    }
}

//--------------------------------------------------------------------------------
//  ����ƥ��ƥ�����������ƥ��ƥ���������������ޤ���
//  �Ƴ�ʵ��
//--------------------------------------------------------------------------------
void EntitySystem::RemoveEntity(Entity* entity)
{
    if (!entity) return;

    auto iterator = m_entities.find(entity->GetId());
    if (iterator != m_entities.end())
    {
        m_entities.erase(iterator);
    }
}
#endif