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

        //--------------------------------------------------------------------------------
        //  calculate the world matrix
        //  ワ�`ルドマトリックスの麻竃
        //  柴麻弊順裳專
        //--------------------------------------------------------------------------------
        void CalculateWorld();

        //--------------------------------------------------------------------------------
        //  setter
        //--------------------------------------------------------------------------------
        void SetPosition(const DirectX::XMVECTORF32& position) { m_position = position; }
        void SetRotation(const DirectX::XMVECTORF32& rotation) { m_rotation = rotation; }
        void SetScalling(const DirectX::XMVECTORF32& scalling) { m_scalling = scalling; }
        void SetLocal(const DirectX::XMVECTORF32& position, const DirectX::XMVECTORF32& rotation, const DirectX::XMVECTORF32& scalling)
        {
            m_position = position;
            m_rotation = rotation;
            m_scalling = scalling;
        }

        //--------------------------------------------------------------------------------
        //  getter
        //--------------------------------------------------------------------------------
        const DirectX::XMVECTORF32& GetPosition() const { return m_position; }
        const DirectX::XMVECTORF32& GetRotation() const { return m_rotation; }
        const DirectX::XMVECTORF32& GetScalling() const { return m_scalling; }
        const DirectX::XMMATRIX& GetWorld() const { return m_world; }

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
        DirectX::XMVECTORF32 m_position = DirectX::g_XMZero;
        DirectX::XMVECTORF32 m_rotation = DirectX::g_XMIdentityR3;
        DirectX::XMVECTORF32 m_scalling = DirectX::g_XMOne3;
        DirectX::XMMATRIX m_world = DirectX::XMMatrixIdentity();
    };
}