//--------------------------------------------------------------------------------
//  renderer component
//  ������`����ݥͥ��
//  �軭���
//
//  Autor  : �� �Ľ�(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <stdint.h>
#include <string>
#include <DirectXMath.h>
#include "../component.h"

namespace KeepFortissimo
{
    //--------------------------------------------------------------------------------
    //  ǰ������
    //--------------------------------------------------------------------------------
    enum class RenderPriority : uint32_t;

    class Renderer : public Component
    {
    public:
        //--------------------------------------------------------------------------------
        //  constructor
        //  ���󥹥ȥ饯��
        //  ���캯��
        //--------------------------------------------------------------------------------
        Renderer(Entity& owner);

        //--------------------------------------------------------------------------------
        //  destructor
        //  �ǥ��ȥ饯��
        //  ��������
        //--------------------------------------------------------------------------------
        virtual ~Renderer() {}

        //--------------------------------------------------------------------------------
        //  setter
        //--------------------------------------------------------------------------------
        void Set(const RenderPriority render_priority) { m_render_priority = render_priority; }
        void Set(const uint32_t object_constant_buffer_index) { m_object_constant_buffer_index = object_constant_buffer_index; }
        void SetMeshGeometryName(const std::string& name) { m_mesh_geometry_name = name; }
        void SetUnitMeshName(const std::string& name) { m_unit_mesh_name = name; }

        //--------------------------------------------------------------------------------
        //  getter
        //--------------------------------------------------------------------------------
        RenderPriority           GetRenderPriority() const { return m_render_priority; }
        const DirectX::XMMATRIX& GetWorld();
        const std::string&       GetMeshGeometryName() const { return m_mesh_geometry_name; }
        const std::string&       GetUnitMeshName() const { return m_unit_mesh_name; }
        uint32_t                 GetObjectConstantBufferIndex() const { return m_object_constant_buffer_index; }
   
    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  ���ԩ`���󥹥ȥ饯���ȥ��ڥ�`���`������
        //  ɾ�������ù��캯����Ⱥ�
        //--------------------------------------------------------------------------------
        Renderer() = delete;
        Renderer(Renderer const&) = delete;
        void operator=(Renderer const&) = delete;

        //--------------------------------------------------------------------------------
        //  variable / ���� / ����
        //--------------------------------------------------------------------------------
        RenderPriority m_render_priority;
        std::string    m_mesh_geometry_name;
        std::string    m_unit_mesh_name;
        uint32_t       m_object_constant_buffer_index = 0;
    };
}