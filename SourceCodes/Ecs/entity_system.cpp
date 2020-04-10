//--------------------------------------------------------------------------------
//  entity system
//  エンティティシステム
//  实体系统
//
//  Autor  : 徐 文杰(Wenjie Xu)
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
//  エンティティ生成
//  生成实体
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
//  インスタンスの終了処理
//  终了处理
//--------------------------------------------------------------------------------
void EntitySystem::Uninitialize()
{
    for (auto pair : m_root_entities)
    {
        SAFE_UNINIT(pair.second);
    }
}

//--------------------------------------------------------------------------------
//  ルートエンティティ追加
//  追加根实体
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
//  ルートエンティティ削除（エンティティの中身は削除しません）
//  移除根实体
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
//  エンティティ追加
//  追加实体
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
//  エンティティ削除（エンティティの中身は削除しません）
//  移除实体
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