//--------------------------------------------------------------------------------
//  generate mesh data
//  メッシュ作成クラス
//  网格生成器
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include "../system_setting.h"
#include "render_define.h"

namespace KeepFortissimo
{
    class MeshGenerator
    {
    public:
        ///<summary>
        /// Creates a box centered at the origin with the given dimensions, where each
        /// face has m rows and n columns of vertices.
        ///</summary>
        static MeshData CreateBox(float width, float height, float depth, uint32_t num_subdivisions);

        ///<summary>
        /// Creates a sphere centered at the origin with the given radius.  The
        /// slices and stacks parameters control the degree of tessellation.
        ///</summary>
        static MeshData CreateSphere(float radius, uint32_t slice_count, uint32_t stack_count);

        ///<summary>
        /// Creates a geosphere centered at the origin with the given radius.  The
        /// depth controls the level of tessellation.
        ///</summary>
        static MeshData CreateGeosphere(float radius, uint32_t num_subdivisions);

        ///<summary>
        /// Creates a cylinder parallel to the y-axis, and centered about the origin.  
        /// The bottom and top radius can vary to form various cone shapes rather than true
        // cylinders.  The slices and stacks parameters control the degree of tessellation.
        ///</summary>
        static MeshData CreateCylinder(float bottom_radius, float top_radius, float height, uint32_t slice_count, uint32_t stack_count);

        ///<summary>
        /// Creates an mxn grid in the xz-plane with m rows and n columns, centered
        /// at the origin with the specified width and depth.
        ///</summary>
        static MeshData CreateGrid(float width, float depth, uint32_t m, uint32_t n);

        ///<summary>
        /// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
        ///</summary>
        static MeshData CreateQuad(float x, float y, float w, float h, float depth);

    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        MeshGenerator() = delete;
        MeshGenerator(const MeshGenerator& rhs) = delete;
        MeshGenerator& operator=(const MeshGenerator& rhs) = delete;
        ~MeshGenerator() = delete;

        static void     Subdivide(MeshData& mesh_data);
        static Vertex3d MidPoint(const Vertex3d& v0, const Vertex3d& v1);
        static void     BuildCylinderTopCap(float bottom_radius, float top_radius, float height, uint32_t slice_count, uint32_t stack_count, MeshData& mesh_data);
        static void     BuildCylinderBottomCap(float bottom_radius, float top_radius, float height, uint32_t slice_count, uint32_t stack_count, MeshData& mesh_data);
    };
}