//--------------------------------------------------------------------------------
//  transform component
//  トランスフォームコンポネント
//  位置情报组件
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <unordered_map>
#include <DirectXMath.h>
#include "component.h"

namespace KeepFortissimo
{
    class Transform : public Component
    {
    public:
        //--------------------------------------------------------------------------------
        //  constructor
        //  コンストラクタ
        //  构造函数
        //--------------------------------------------------------------------------------
        Transform(Entity& owner);

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  析构函数
        //--------------------------------------------------------------------------------
        virtual ~Transform() {}

    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        Transform() = delete;
        Transform(Transform const&) = delete;
        void operator=(Transform const&) = delete;

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        DirectX::XMVECTOR m_position = DirectX::XMVectorZero();
        DirectX::XMVECTOR m_rotation = DirectX::XMQuaternionIdentity();
        DirectX::XMVECTOR m_scalling = DirectX::g_XMOne3;
        DirectX::XMMATRIX m_world = DirectX::XMMatrixIdentity();
    };
}