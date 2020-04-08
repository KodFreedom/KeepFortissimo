//--------------------------------------------------------------------------------
//  transform component
//  �ȥ�󥹥ե��`�ॳ��ݥͥ��
//  λ���鱨���
//
//  Autor  : �� �Ľ�(Wenjie Xu)
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
        //  ���󥹥ȥ饯��
        //  ���캯��
        //--------------------------------------------------------------------------------
        Transform(Entity& owner);

        //--------------------------------------------------------------------------------
        //  destructor
        //  �ǥ��ȥ饯��
        //  ��������
        //--------------------------------------------------------------------------------
        virtual ~Transform() {}

    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  ���ԩ`���󥹥ȥ饯���ȥ��ڥ�`���`������
        //  ɾ�������ù��캯����Ⱥ�
        //--------------------------------------------------------------------------------
        Transform() = delete;
        Transform(Transform const&) = delete;
        void operator=(Transform const&) = delete;

        //--------------------------------------------------------------------------------
        //  variable / ���� / ����
        //--------------------------------------------------------------------------------
        DirectX::XMVECTOR m_position = DirectX::XMVectorZero();
        DirectX::XMVECTOR m_rotation = DirectX::XMQuaternionIdentity();
        DirectX::XMVECTOR m_scalling = DirectX::g_XMOne3;
        DirectX::XMMATRIX m_world = DirectX::XMMatrixIdentity();
    };
}