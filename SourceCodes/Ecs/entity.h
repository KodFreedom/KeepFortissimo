//--------------------------------------------------------------------------------
//  entity class
//  エンティティクラス
//  实体类
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <unordered_map>
#include "Components/component.h"
#include "Components/component_define.h"

namespace KeepFortissimo
{
    class Entity
    {
        friend class EntitySystem;

    public:
        //--------------------------------------------------------------------------------
        //  GetComponent
        //  コンポーネント取得
        //  取得组件
        //--------------------------------------------------------------------------------
        template<class T>
        T* GetComponent()
        {
            //static_assert(std::is_base_of<Component, T>);
            std::string type_name = TypeName<T>::Get();

            auto result = m_components.find(type_name);
            if (result == m_components.end())
            {
                return nullptr;
            }

            return static_cast<T*>(result->second);
        }

        //--------------------------------------------------------------------------------
        //  AddComponent
        //  コンポーネントの生成
        //  生成组件
        //--------------------------------------------------------------------------------
        template<class T>
        T* AddComponent()
        {
            //static_assert(std::is_base_of<Component, T>);
            std::string type_name = TypeName<T>::Get();

            auto iterator = m_components.find(type_name);
            if (iterator != m_components.end())
            {// TODO : Assert
                return nullptr;
            }

            T* result = MY_NEW T(*this);
            m_components.emplace(type_name, static_cast<Component*>(result));
            result->Initialize();

            return result;
        }

        //--------------------------------------------------------------------------------
        //  RemoveComponent
        //  コンポーネントの削除
        //  删除组件
        //--------------------------------------------------------------------------------
        template<class T>
        void RemoveComponent()
        {
            //static_assert(std::is_base_of<Component, T>);
            std::string type_name = TypeName<T>::Get();

            auto result = m_components.find(type_name);
            if (result != m_components.end())
            {
                SAFE_UNINIT(result->second);
                m_components.erase(result);
            }
        }

        //--------------------------------------------------------------------------------
        //  親登録処理
        //--------------------------------------------------------------------------------
        void SetParent(Entity* parent);

        //--------------------------------------------------------------------------------
        //  子供登録処理
        //--------------------------------------------------------------------------------
        void AddChild(Entity* child);

        //--------------------------------------------------------------------------------
        //  子供削除処理
        //--------------------------------------------------------------------------------
        void RemoveChild(Entity* child);

        //--------------------------------------------------------------------------------
        //  Getter
        //-------------------------------------------------------------------------------
        uint32_t GetId() const { return m_id; }

    private:
        //--------------------------------------------------------------------------------
        //  constructor
        //  コンストラクタ
        //  构造函数
        //--------------------------------------------------------------------------------
        Entity(const uint32_t id);

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  析构函数
        //--------------------------------------------------------------------------------
        ~Entity();

        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        Entity() = delete;
        Entity(Entity const&) = delete;
        void operator=(Entity const&) = delete;

        //--------------------------------------------------------------------------------
        //  初期化処理
        //  初始化
        //--------------------------------------------------------------------------------
        void Initialize() {}

        //--------------------------------------------------------------------------------
        //  終了処理
        //  终止
        //--------------------------------------------------------------------------------
        void Uninitialize();

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        const uint32_t m_id;
        std::unordered_map<std::string, Component*> m_components = {};
        Entity* m_parent = nullptr;
        std::unordered_map<uint32_t, Entity*> m_children;
    };
}