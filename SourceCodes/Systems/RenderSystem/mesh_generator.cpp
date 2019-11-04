//--------------------------------------------------------------------------------
//  generate mesh data
//  ÉÅÉbÉVÉÖçÏê¨ÉNÉâÉX
//  „§äiê∂ê¨äÌ
//
//  Autor  : èô ï∂û^(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "mesh_generator.h"
#include <algorithm>
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
///<summary>
/// Creates a box centered at the origin with the given dimensions, where each
/// face has m rows and n columns of vertices.
///</summary>
MeshData MeshGenerator::CreateBox(float width, float height, float depth, u32 num_subdivisions)
{
    MeshData mesh_data;

    //
    // Create the vertices.
    //

    Vertex3d v[24];

    float w2 = 0.5f * width;
    float h2 = 0.5f * height;
    float d2 = 0.5f * depth;

    // Fill in the front face vertex data.
    v[0] = Vertex3d({ XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });
    v[1] = Vertex3d({ XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
    v[2] = Vertex3d({ XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) });
    v[3] = Vertex3d({ XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) });

    // Fill in the back face vertex data.
    v[4] = Vertex3d({ XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) });
    v[5] = Vertex3d({ XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });
    v[6] = Vertex3d({ XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
    v[7] = Vertex3d({ XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) });

    // Fill in the top face vertex data.
    v[8]  = Vertex3d({ XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });
    v[9]  = Vertex3d({ XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
    v[10] = Vertex3d({ XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) });
    v[11] = Vertex3d({ XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) });

    // Fill in the bottom face vertex data.
    v[12] = Vertex3d({ XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) });
    v[13] = Vertex3d({ XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });
    v[14] = Vertex3d({ XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
    v[15] = Vertex3d({ XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) });

    // Fill in the left face vertex data.
    v[16] = Vertex3d({ XMFLOAT3(-w2, -h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) });
    v[17] = Vertex3d({ XMFLOAT3(-w2, +h2, +d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) });
    v[18] = Vertex3d({ XMFLOAT3(-w2, +h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) });
    v[19] = Vertex3d({ XMFLOAT3(-w2, -h2, -d2), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) });
                                                
    // Fill in the right face vertex data.      
    v[20] = Vertex3d({ XMFLOAT3(+w2, -h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) });
    v[21] = Vertex3d({ XMFLOAT3(+w2, +h2, -d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) });
    v[22] = Vertex3d({ XMFLOAT3(+w2, +h2, +d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) });
    v[23] = Vertex3d({ XMFLOAT3(+w2, -h2, +d2), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) });

    mesh_data.vertices.assign(&v[0], &v[24]);

    //
    // Create the indices.
    //

    u32 i[36];

    // Fill in the front face index data
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;

    // Fill in the back face index data
    i[6] = 4; i[7] = 5; i[8] = 6;
    i[9] = 4; i[10] = 6; i[11] = 7;

    // Fill in the top face index data
    i[12] = 8; i[13] = 9; i[14] = 10;
    i[15] = 8; i[16] = 10; i[17] = 11;

    // Fill in the bottom face index data
    i[18] = 12; i[19] = 13; i[20] = 14;
    i[21] = 12; i[22] = 14; i[23] = 15;

    // Fill in the left face index data
    i[24] = 16; i[25] = 17; i[26] = 18;
    i[27] = 16; i[28] = 18; i[29] = 19;

    // Fill in the right face index data
    i[30] = 20; i[31] = 21; i[32] = 22;
    i[33] = 20; i[34] = 22; i[35] = 23;

    mesh_data.indeces.assign(&i[0], &i[36]);

    // Put a cap on the number of subdivisions.
    num_subdivisions = std::min<u32>(num_subdivisions, 6u);

    for (u32 i = 0; i < num_subdivisions; ++i)
    {
        Subdivide(mesh_data);
    }

    return mesh_data;
}

///<summary>
/// Creates a sphere centered at the origin with the given radius.  The
/// slices and stacks parameters control the degree of tessellation.
///</summary>
MeshData MeshGenerator::CreateSphere(float radius, u32 slice_count, u32 stack_count)
{
    MeshData mesh_data;

    //
    // Compute the vertices stating at the top pole and moving down the stacks.
    //

    // Poles: note that there will be texture coordinate distortion as there is
    // not a unique point on the texture map to assign to the pole when mapping
    // a rectangular texture onto a sphere.
    Vertex3d top_vertex    = Vertex3d({ XMFLOAT3(0.0f, +radius, 0.0f), XMFLOAT3(0.0f, +1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) });
    Vertex3d bottom_vertex = Vertex3d({ XMFLOAT3(0.0f, -radius, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) });

    mesh_data.vertices.push_back(top_vertex);

    float phi_step = XM_PI / stack_count;
    float theta_step = 2.0f * XM_PI / slice_count;

    // Compute vertices for each stack ring (do not count the poles as rings).
    for (u32 i = 1; i <= stack_count - 1; ++i)
    {
        float phi = i * phi_step;

        // vertices of ring.
        for (u32 j = 0; j <= slice_count; ++j)
        {
            float theta = j * theta_step;

            Vertex3d v;

            // spherical to cartesian
            v.position.x = radius * sinf(phi) * cosf(theta);
            v.position.y = radius * cosf(phi);
            v.position.z = radius * sinf(phi) * sinf(theta);

            // Partial derivative of P with respect to theta
            v.tangent.x = -radius * sinf(phi) * sinf(theta);
            v.tangent.y = 0.0f;
            v.tangent.z = +radius * sinf(phi) * cosf(theta);

            XMVECTOR T = XMLoadFloat3(&v.tangent);
            XMStoreFloat3(&v.tangent, XMVector3Normalize(T));

            XMVECTOR p = XMLoadFloat3(&v.position);
            XMStoreFloat3(&v.normal, XMVector3Normalize(p));

            v.uv.x = theta / XM_2PI;
            v.uv.y = phi / XM_PI;

            mesh_data.vertices.push_back(v);
        }
    }

    mesh_data.vertices.push_back(bottom_vertex);

    //
    // Compute indices for top stack.  The top stack was written first to the vertex buffer
    // and connects the top pole to the first ring.
    //

    for (u32 i = 1; i <= slice_count; ++i)
    {
        mesh_data.indeces.push_back(0);
        mesh_data.indeces.push_back(i + 1);
        mesh_data.indeces.push_back(i);
    }

    //
    // Compute indices for inner stacks (not connected to poles).
    //

    // Offset the indices to the index of the first vertex in the first ring.
    // This is just skipping the top pole vertex.
    u32 base_index = 1;
    u32 ring_vertex_count = slice_count + 1;
    for (u32 i = 0; i < stack_count - 2; ++i)
    {
        for (u32 j = 0; j < slice_count; ++j)
        {
            mesh_data.indeces.push_back(base_index + i * ring_vertex_count + j);
            mesh_data.indeces.push_back(base_index + i * ring_vertex_count + j + 1);
            mesh_data.indeces.push_back(base_index + (i + 1) * ring_vertex_count + j);

            mesh_data.indeces.push_back(base_index + (i + 1) * ring_vertex_count + j);
            mesh_data.indeces.push_back(base_index + i * ring_vertex_count + j + 1);
            mesh_data.indeces.push_back(base_index + (i + 1) * ring_vertex_count + j + 1);
        }
    }

    //
    // Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
    // and connects the bottom pole to the bottom ring.
    //

    // South pole vertex was added last.
    u32 south_pole_index = static_cast<u32>(mesh_data.vertices.size() - 1);

    // Offset the indices to the index of the first vertex in the last ring.
    base_index = south_pole_index - ring_vertex_count;

    for (u32 i = 0; i < slice_count; ++i)
    {
        mesh_data.indeces.push_back(south_pole_index);
        mesh_data.indeces.push_back(base_index + i);
        mesh_data.indeces.push_back(base_index + i + 1);
    }

    return mesh_data;
}

///<summary>
/// Creates a geosphere centered at the origin with the given radius.  The
/// depth controls the level of tessellation.
///</summary>
MeshData MeshGenerator::CreateGeosphere(float radius, u32 num_subdivisions)
{
    MeshData mesh_data;

    // Put a cap on the number of subdivisions.
    num_subdivisions = std::min<u32>(num_subdivisions, 6u);

    // Approximate a sphere by tessellating an icosahedron.

    const float X = 0.525731f;
    const float Z = 0.850651f;

    XMFLOAT3 pos[12] =
    {
        XMFLOAT3(-X, 0.0f, Z),  XMFLOAT3(X, 0.0f, Z),
        XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
        XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
        XMFLOAT3(0.0f, -Z, X),  XMFLOAT3(0.0f, -Z, -X),
        XMFLOAT3(Z, X, 0.0f),   XMFLOAT3(-Z, X, 0.0f),
        XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)
    };

    u32 k[60] =
    {
        1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
        1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
        3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
        10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
    };

    mesh_data.vertices.resize(12);
    mesh_data.indeces.assign(&k[0], &k[60]);

    for (u32 i = 0; i < 12; ++i)
    {
        mesh_data.vertices[i].position = pos[i];
    }

    for (u32 i = 0; i < num_subdivisions; ++i)
    {
        Subdivide(mesh_data);
    }

    // Project vertices onto sphere and scale.
    for (u32 i = 0; i < mesh_data.vertices.size(); ++i)
    {
        // Project onto unit sphere.
        XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mesh_data.vertices[i].position));

        // Project onto sphere.
        XMVECTOR p = radius * n;

        XMStoreFloat3(&mesh_data.vertices[i].position, p);
        XMStoreFloat3(&mesh_data.vertices[i].normal, n);

        // Derive texture coordinates from spherical coordinates.
        float theta = atan2f(mesh_data.vertices[i].position.z, mesh_data.vertices[i].position.x);

        // Put in [0, 2pi].
        if (theta < 0.0f) theta += XM_2PI;

        float phi = acosf(mesh_data.vertices[i].position.y / radius);

        mesh_data.vertices[i].uv.x = theta / XM_2PI;
        mesh_data.vertices[i].uv.y = phi / XM_PI;

        // Partial derivative of P with respect to theta
        mesh_data.vertices[i].tangent.x = -radius * sinf(phi) * sinf(theta);
        mesh_data.vertices[i].tangent.y = 0.0f;
        mesh_data.vertices[i].tangent.z = +radius * sinf(phi) * cosf(theta);

        XMVECTOR T = XMLoadFloat3(&mesh_data.vertices[i].tangent);
        XMStoreFloat3(&mesh_data.vertices[i].tangent, XMVector3Normalize(T));
    }

    return mesh_data;
}

///<summary>
/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
/// The bottom and top radius can vary to form various cone shapes rather than true
// cylinders.  The slices and stacks parameters control the degree of tessellation.
///</summary>
MeshData MeshGenerator::CreateCylinder(float bottom_radius, float top_radius, float height, u32 slice_count, u32 stack_count)
{
    MeshData mesh_data;

    //
    // Build Stacks.
    // 

    float stackHeight = height / stack_count;

    // Amount to increment radius as we move up each stack level from bottom to top.
    float radiusStep = (top_radius - bottom_radius) / stack_count;

    u32 ringCount = stack_count + 1;

    // Compute vertices for each stack ring starting at the bottom and moving up.
    for (u32 i = 0; i < ringCount; ++i)
    {
        float y = -0.5f * height + i * stackHeight;
        float r = bottom_radius + i * radiusStep;

        // vertices of ring
        float d_theta = 2.0f * XM_PI / slice_count;
        for (u32 j = 0; j <= slice_count; ++j)
        {
            Vertex3d vertex;

            float c = cosf(j * d_theta);
            float s = sinf(j * d_theta);

            vertex.position = XMFLOAT3(r * c, y, r * s);

            vertex.uv.x = (float)j / slice_count;
            vertex.uv.y = 1.0f - (float)i / stack_count;

            // Cylinder can be parameterized as follows, where we introduce v
            // parameter that goes in the same direction as the v tex-coord
            // so that the bitangent goes in the same direction as the v tex-coord.
            //   Let r0 be the bottom radius and let r1 be the top radius.
            //   y(v) = h - hv for v in [0,1].
            //   r(v) = r1 + (r0-r1)v
            //
            //   x(t, v) = r(v)*cos(t)
            //   y(t, v) = h - hv
            //   z(t, v) = r(v)*sin(t)
            // 
            //  dx/dt = -r(v)*sin(t)
            //  dy/dt = 0
            //  dz/dt = +r(v)*cos(t)
            //
            //  dx/dv = (r0-r1)*cos(t)
            //  dy/dv = -h
            //  dz/dv = (r0-r1)*sin(t)

            // This is unit length.
            vertex.tangent = XMFLOAT3(-s, 0.0f, c);

            float dr = bottom_radius - top_radius;
            XMFLOAT3 bitangent(dr * c, -height, dr * s);

            XMVECTOR T = XMLoadFloat3(&vertex.tangent);
            XMVECTOR B = XMLoadFloat3(&bitangent);
            XMVECTOR N = XMVector3Normalize(XMVector3Cross(T, B));
            XMStoreFloat3(&vertex.normal, N);

            mesh_data.vertices.push_back(vertex);
        }
    }

    // Add one because we duplicate the first and last vertex per ring
    // since the texture coordinates are different.
    u32 ringVertexCount = slice_count + 1;

    // Compute indices for each stack.
    for (u32 i = 0; i < stack_count; ++i)
    {
        for (u32 j = 0; j < slice_count; ++j)
        {
            mesh_data.indeces.push_back(i * ringVertexCount + j);
            mesh_data.indeces.push_back((i + 1) * ringVertexCount + j);
            mesh_data.indeces.push_back((i + 1) * ringVertexCount + j + 1);

            mesh_data.indeces.push_back(i * ringVertexCount + j);
            mesh_data.indeces.push_back((i + 1) * ringVertexCount + j + 1);
            mesh_data.indeces.push_back(i * ringVertexCount + j + 1);
        }
    }

    BuildCylinderTopCap(bottom_radius, top_radius, height, slice_count, stack_count, mesh_data);
    BuildCylinderBottomCap(bottom_radius, top_radius, height, slice_count, stack_count, mesh_data);

    return mesh_data;
}

///<summary>
/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
/// at the origin with the specified width and depth.
///</summary>
MeshData MeshGenerator::CreateGrid(float width, float depth, u32 m, u32 n)
{
    MeshData mesh_data;

    u32 vertexCount = m * n;
    u32 faceCount = (m - 1) * (n - 1) * 2;

    //
    // Create the vertices.
    //

    float halfWidth = 0.5f * width;
    float halfDepth = 0.5f * depth;

    float dx = width / (n - 1);
    float dz = depth / (m - 1);

    float du = 1.0f / (n - 1);
    float dv = 1.0f / (m - 1);

    mesh_data.vertices.resize(vertexCount);
    for (u32 i = 0; i < m; ++i)
    {
        float z = halfDepth - i * dz;
        for (u32 j = 0; j < n; ++j)
        {
            float x = -halfWidth + j * dx;

            mesh_data.vertices[i * n + j].position = XMFLOAT3(x, 0.0f, z);
            mesh_data.vertices[i * n + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
            mesh_data.vertices[i * n + j].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

            // Stretch texture over grid.
            mesh_data.vertices[i * n + j].uv.x = j * du;
            mesh_data.vertices[i * n + j].uv.y = i * dv;
        }
    }

    //
    // Create the indices.
    //

    mesh_data.indeces.resize(faceCount * 3); // 3 indices per face

    // Iterate over each quad and compute indices.
    u32 k = 0;
    for (u32 i = 0; i < m - 1; ++i)
    {
        for (u32 j = 0; j < n - 1; ++j)
        {
            mesh_data.indeces[k] = i * n + j;
            mesh_data.indeces[k + 1] = i * n + j + 1;
            mesh_data.indeces[k + 2] = (i + 1) * n + j;

            mesh_data.indeces[k + 3] = (i + 1) * n + j;
            mesh_data.indeces[k + 4] = i * n + j + 1;
            mesh_data.indeces[k + 5] = (i + 1) * n + j + 1;

            k += 6; // next quad
        }
    }

    return mesh_data;
}

///<summary>
/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
///</summary>
MeshData MeshGenerator::CreateQuad(float x, float y, float w, float h, float depth)
{
    MeshData mesh_data;

    mesh_data.vertices.resize(4);
    mesh_data.indeces.resize(6);

    // position coordinates specified in NDC space.
    mesh_data.vertices[0] = Vertex3d({
        XMFLOAT3(x, y - h, depth),
        XMFLOAT3(0.0f, 0.0f, -1.0f),
        XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT2(0.0f, 1.0f) });

    mesh_data.vertices[1] = Vertex3d({
        XMFLOAT3(x, y, depth),
        XMFLOAT3(0.0f, 0.0f, -1.0f),
        XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT2(0.0f, 0.0f) });

    mesh_data.vertices[2] = Vertex3d({
        XMFLOAT3(x + w, y, depth),
        XMFLOAT3(0.0f, 0.0f, -1.0f),
        XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT2(1.0f, 0.0f) });

    mesh_data.vertices[3] = Vertex3d({
        XMFLOAT3(x + w, y - h, depth),
        XMFLOAT3(0.0f, 0.0f, -1.0f),
        XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT2(1.0f, 1.0f) });

    mesh_data.indeces[0] = 0;
    mesh_data.indeces[1] = 1;
    mesh_data.indeces[2] = 2;

    mesh_data.indeces[3] = 0;
    mesh_data.indeces[4] = 2;
    mesh_data.indeces[5] = 3;

    return mesh_data;
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
void MeshGenerator::Subdivide(MeshData& mesh_data)
{
    // Save a copy of the input geometry.
    MeshData mesh_copy = mesh_data;
    mesh_data.vertices.resize(0);
    mesh_data.indeces.resize(0);

    //       v1
    //       *
    //      / \
	//     /   \
	//  m0*-----*m1
    //   / \   / \
	//  /   \ /   \
	// *-----*-----*
    // v0    m2     v2
    u32 num_triangles = static_cast<u32>(mesh_copy.indeces.size() / 3);
    for (u32 i = 0; i < num_triangles; ++i)
    {
        Vertex3d v0 = mesh_copy.vertices[mesh_copy.indeces[i * 3 + 0]];
        Vertex3d v1 = mesh_copy.vertices[mesh_copy.indeces[i * 3 + 1]];
        Vertex3d v2 = mesh_copy.vertices[mesh_copy.indeces[i * 3 + 2]];

        //
        // Generate the midpoints.
        //
        Vertex3d m0 = MidPoint(v0, v1);
        Vertex3d m1 = MidPoint(v1, v2);
        Vertex3d m2 = MidPoint(v0, v2);

        //
        // Add new geometry.
        //
        mesh_data.vertices.push_back(v0); // 0
        mesh_data.vertices.push_back(v1); // 1
        mesh_data.vertices.push_back(v2); // 2
        mesh_data.vertices.push_back(m0); // 3
        mesh_data.vertices.push_back(m1); // 4
        mesh_data.vertices.push_back(m2); // 5

        mesh_data.indeces.push_back(i * 6 + 0);
        mesh_data.indeces.push_back(i * 6 + 3);
        mesh_data.indeces.push_back(i * 6 + 5);

        mesh_data.indeces.push_back(i * 6 + 3);
        mesh_data.indeces.push_back(i * 6 + 4);
        mesh_data.indeces.push_back(i * 6 + 5);

        mesh_data.indeces.push_back(i * 6 + 5);
        mesh_data.indeces.push_back(i * 6 + 4);
        mesh_data.indeces.push_back(i * 6 + 2);

        mesh_data.indeces.push_back(i * 6 + 3);
        mesh_data.indeces.push_back(i * 6 + 1);
        mesh_data.indeces.push_back(i * 6 + 4);
    }
}

Vertex3d MeshGenerator::MidPoint(const Vertex3d& v0, const Vertex3d& v1)
{
    XMVECTOR position0 = XMLoadFloat3(&v0.position);
    XMVECTOR position1 = XMLoadFloat3(&v1.position);

    XMVECTOR normal0 = XMLoadFloat3(&v0.normal);
    XMVECTOR normal1 = XMLoadFloat3(&v1.normal);

    XMVECTOR tangent0 = XMLoadFloat3(&v0.tangent);
    XMVECTOR tangent1 = XMLoadFloat3(&v1.tangent);

    XMVECTOR uv0 = XMLoadFloat2(&v0.uv);
    XMVECTOR uv1 = XMLoadFloat2(&v1.uv);

    // Compute the midpoints of all the attributes.  Vectors need to be normalized
    // since linear interpolating can make them not unit length.  
    XMVECTOR position = 0.5f * (position0 + position1);
    XMVECTOR normal = XMVector3Normalize(0.5f * (normal0 + normal1));
    XMVECTOR tangent = XMVector3Normalize(0.5f * (tangent0 + tangent1));
    XMVECTOR uv = 0.5f * (uv0 + uv1);

    Vertex3d v;
    XMStoreFloat3(&v.position, position);
    XMStoreFloat3(&v.normal, normal);
    XMStoreFloat3(&v.tangent, tangent);
    XMStoreFloat2(&v.uv, uv);

    return v;
}

void MeshGenerator::BuildCylinderTopCap(float bottom_radius, float top_radius, float height, u32 slice_count, u32 stack_count, MeshData& mesh_data)
{
    u32 base_index = static_cast<u32>(mesh_data.vertices.size());

    float y = 0.5f * height;
    float d_theta = 2.0f * XM_PI / slice_count;

    // Duplicate cap ring vertices because the texture coordinates and normals differ.
    for (u32 i = 0; i <= slice_count; ++i)
    {
        float x = top_radius * cosf(i * d_theta);
        float z = top_radius * sinf(i * d_theta);

        // Scale down by the height to try and make top cap texture coord area
        // proportional to base.
        float u = x / height + 0.5f;
        float v = z / height + 0.5f;

        mesh_data.vertices.push_back(Vertex3d({
            XMFLOAT3(x, y, z),
            XMFLOAT3(0.0f, 1.0f, 0.0f),
            XMFLOAT3(1.0f, 0.0f, 0.0f),
            XMFLOAT2(u, v) }));
    }

    // Cap center vertex.
    mesh_data.vertices.push_back(Vertex3d({
        XMFLOAT3(0.0f, y, 0.0f),
        XMFLOAT3(0.0f, 1.0f, 0.0f),
        XMFLOAT3(1.0f, 0.0f, 0.0f),
        XMFLOAT2(0.5f, 0.5f) }));

    // Index of center vertex.
    u32 center_index = (u32)mesh_data.vertices.size() - 1;

    for (u32 i = 0; i < slice_count; ++i)
    {
        mesh_data.indeces.push_back(center_index);
        mesh_data.indeces.push_back(base_index + i + 1);
        mesh_data.indeces.push_back(base_index + i);
    }
}

void MeshGenerator::BuildCylinderBottomCap(float bottom_radius, float top_radius, float height, u32 slice_count, u32 stack_count, MeshData& mesh_data)
{
    // 
    // Build bottom cap.
    //

    u32 base_index = static_cast<u32>(mesh_data.vertices.size());
    float y = -0.5f * height;

    // vertices of ring
    float d_theta = 2.0f * XM_PI / slice_count;
    for (u32 i = 0; i <= slice_count; ++i)
    {
        float x = bottom_radius * cosf(i * d_theta);
        float z = bottom_radius * sinf(i * d_theta);

        // Scale down by the height to try and make top cap texture coord area
        // proportional to base.
        float u = x / height + 0.5f;
        float v = z / height + 0.5f;

        mesh_data.vertices.push_back(Vertex3d({
            XMFLOAT3(x, y, z),
            XMFLOAT3(0.0f, -1.0f, 0.0f),
            XMFLOAT3(1.0f, 0.0f, 0.0f),
            XMFLOAT2(u, v) }));
    }

    // Cap center vertex.
    mesh_data.vertices.push_back(Vertex3d({
         XMFLOAT3(0.0f, y, 0.0f),
         XMFLOAT3(0.0f, -1.0f, 0.0f),
         XMFLOAT3(1.0f, 0.0f, 0.0f),
         XMFLOAT2(0.5f, 0.5f) }));

    // Cache the index of center vertex.
    u32 center_index = static_cast<u32>(mesh_data.vertices.size() - 1);

    for (u32 i = 0; i < slice_count; ++i)
    {
        mesh_data.indeces.push_back(center_index);
        mesh_data.indeces.push_back(base_index + i);
        mesh_data.indeces.push_back(base_index + i + 1);
    }
}