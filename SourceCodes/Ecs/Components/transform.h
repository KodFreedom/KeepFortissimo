//--------------------------------------------------------------------------------
//  transform component
//  トランスフォ�`ムコンポネント
//  了崔秤烏怏周
//
//  Autor  : 俾 猟旬(Wenjie Xu)
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
        //  更夛痕方
        //--------------------------------------------------------------------------------
        Transform(Entity& owner);

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  裂更痕方
        //--------------------------------------------------------------------------------
        virtual ~Transform() {}

    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピ�`コンストラクタとオペレ�`タ�`の��茅
        //  評茅鹸崙喘更夛痕方嚥吉催
        //--------------------------------------------------------------------------------
        Transform() = delete;
        Transform(Transform const&) = delete;
        void operator=(Transform const&) = delete;

        //--------------------------------------------------------------------------------
        //  variable / �篳� / 延楚
        //--------------------------------------------------------------------------------
        DirectX::XMVECTOR m_position = DirectX::XMVectorZero();
        DirectX::XMVECTOR m_rotation = DirectX::XMQuaternionIdentity();
        DirectX::XMVECTOR m_scalling = DirectX::g_XMOne3;
        DirectX::XMMATRIX m_world = DirectX::XMMatrixIdentity();
    };
}