//--------------------------------------------------------------------------------
//  main system
//  メインシステム
//  主系统
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "main_system.h"
#include "game_timer.h"
#include "camera_system.h"
#include "../Utilities/kf_labels.h"
#include "RenderSystem/render_system.h"
#include "../Ecs/entity_system.h"
#include "../Ecs/entity.h"
#include "../Ecs/Components/transform.h"
#include "../Ecs/Components/Renderer/renderer.h"
using namespace KeepFortissimo;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    // Forward hwnd on because we can get messages (e.g., WM_CREATE)
    // before CreateWindow returns, and thus before mhMainWnd is valid.
    return MainSystem::Instance().MsgProc(hwnd, msg, wparam, lparam);
}

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  StartUp
//--------------------------------------------------------------------------------
bool MainSystem::StartUp(HINSTANCE instance_handle)
{
    if (m_instance != nullptr) return true;
    MY_NEW MainSystem(instance_handle);
    return m_instance->Initialize();
}

//--------------------------------------------------------------------------------
//  Run
//--------------------------------------------------------------------------------
int MainSystem::Run()
{
    MSG msg;

    // メッセージループ
    while (1)
    {
        // メッセージを取得しなかった場合"0"を返す
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
        {// Windowsの処理
            if (msg.message == WM_QUIT)
            {// PostQuitMessage()が呼ばれて、WM_QUITメッセージが来たらループ終了
                break;
            }
            else
            {// メッセージの翻訳と送出
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            GameTimer& game_timer = GameTimer::Instance();
            game_timer.Tick();

            if (game_timer.CanUpdateFrame())
            {
                Update();
                Render();
            }
        }
    }

    // ウィンドウクラスの登録を解除
    UnregisterClass(kClassName, m_instance_handle);

    return (int)msg.wParam;
}

//--------------------------------------------------------------------------------
//  MsgProc
//--------------------------------------------------------------------------------
LRESULT MainSystem::MsgProc(HWND hwnd, uint32_t msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        // WM_ACTIVATE is sent when the window is activated or deactivated.  
    case WM_ACTIVATE:
        OnWmActivate(wparam);
        return 0;

        // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
        OnWmSize(wparam, lparam);
        return 0;

        // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
    case WM_ENTERSIZEMOVE:
        OnWmEnterSizeMove();
        return 0;

        // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
    case WM_EXITSIZEMOVE:
        OnWmExitSizeMove();
        return 0;

        // WM_DESTROY is sent when the window is being destroyed.
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

        // The WM_MENUCHAR message is sent when a menu is active and the user presses 
        // a key that does not correspond to any mnemonic or accelerator key. 
    case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

        // Catch this message so to prevent the window from becoming too small.
    case WM_GETMINMAXINFO:
        ((MINMAXINFO*)lparam)->ptMinTrackSize.x = 200;
        ((MINMAXINFO*)lparam)->ptMinTrackSize.y = 200;
        return 0;

    case WM_KEYUP:
        OnWmKeyUp(wparam, lparam);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  MainSystem
//--------------------------------------------------------------------------------
MainSystem::MainSystem(HINSTANCE instance_handle)
    : Singleton<MainSystem>()
    , m_instance_handle(instance_handle)
{
}

//--------------------------------------------------------------------------------
//  ~MainSystem
//--------------------------------------------------------------------------------
MainSystem::~MainSystem()
{
}

//--------------------------------------------------------------------------------
//  initialize the instance
//  Return：true when succeeded, else false
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  初期化処理
//  戻り値：成功したらtrue、失敗したらfalse
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  初始化
//  返回值：成功则返回true、反之返回false
//--------------------------------------------------------------------------------
bool MainSystem::Initialize()
{
    GetSystemLanguage();
    
    if (!GameTimer::StartUp())
    {
        return false;
    }

    if (!InitializeWindow())
    {
        return false;
    }

    if (!EntitySystem::StartUp())
    {
        return false;
    }

    if (!CameraSystem::StartUp())
    {
        return false;
    }

    if (!RenderSystem::StartUp(RenderApiType::kDirectX12))
    {
        return false;
    }

    {// test entity
        Entity* box = EntitySystem::Instance().CreateEntity();
        box->GetTransform().SetLocal(
            { 0.0f, 0.0f, 0.0f, 0.0f }, 
            DirectX::g_XMIdentityR3, 
            { 1.0f, 2.0f, 1.0f, 0.0f });
        Renderer* box_renderer = box->AddComponent<Renderer>();
        box_renderer->SetMeshGeometryName("shapeGeo");
        box_renderer->SetUnitMeshName("box");

        Entity* sphere = EntitySystem::Instance().CreateEntity(box);
        sphere->GetTransform().SetLocal(
            { 0.0f, 1.0f, 0.0f, 0.0f }, 
            DirectX::g_XMIdentityR3, 
            { 1.0f, 1.0f, 1.0f, 0.0f });
        Renderer* sphere_renderer = sphere->AddComponent<Renderer>();
        sphere_renderer->SetMeshGeometryName("shapeGeo");
        sphere_renderer->SetUnitMeshName("sphere");

    //auto gridRitem = MY_NEW RenderItem();
    //gridRitem->World = XMMatrixIdentity();
    //gridRitem->ObjCBIndex = 1;
    //gridRitem->Geo = mGeometries["shapeGeo"];
    //gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //gridRitem->IndexCount = gridRitem->Geo->unit_mesh_geometries["grid"].index_count;
    //gridRitem->StartIndexLocation = gridRitem->Geo->unit_mesh_geometries["grid"].start_index_location;
    //gridRitem->BaseVertexLocation = gridRitem->Geo->unit_mesh_geometries["grid"].base_vertex_location;
    //mAllRitems.push_back(gridRitem);

    //uint32_t objCBIndex = 2;
    //for (int i = 0; i < 5; ++i)
    //{
    //    auto leftCylRitem = MY_NEW RenderItem();
    //    auto rightCylRitem = MY_NEW RenderItem();
    //    auto leftSphereRitem = MY_NEW RenderItem();
    //    auto rightSphereRitem = MY_NEW RenderItem();

    //    XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
    //    XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

    //    XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
    //    XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

    //    leftCylRitem->World = rightCylWorld;
    //    leftCylRitem->ObjCBIndex = objCBIndex++;
    //    leftCylRitem->Geo = mGeometries["shapeGeo"];
    //    leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //    leftCylRitem->IndexCount = leftCylRitem->Geo->unit_mesh_geometries["cylinder"].index_count;
    //    leftCylRitem->StartIndexLocation = leftCylRitem->Geo->unit_mesh_geometries["cylinder"].start_index_location;
    //    leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->unit_mesh_geometries["cylinder"].base_vertex_location;

    //    rightCylRitem->World = leftCylWorld;
    //    rightCylRitem->ObjCBIndex = objCBIndex++;
    //    rightCylRitem->Geo = mGeometries["shapeGeo"];
    //    rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //    rightCylRitem->IndexCount = rightCylRitem->Geo->unit_mesh_geometries["cylinder"].index_count;
    //    rightCylRitem->StartIndexLocation = rightCylRitem->Geo->unit_mesh_geometries["cylinder"].start_index_location;
    //    rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->unit_mesh_geometries["cylinder"].base_vertex_location;

    //    leftSphereRitem->World = leftSphereWorld;
    //    leftSphereRitem->ObjCBIndex = objCBIndex++;
    //    leftSphereRitem->Geo = mGeometries["shapeGeo"];
    //    leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //    leftSphereRitem->IndexCount = leftSphereRitem->Geo->unit_mesh_geometries["sphere"].index_count;
    //    leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->unit_mesh_geometries["sphere"].start_index_location;
    //    leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->unit_mesh_geometries["sphere"].base_vertex_location;

    //    rightSphereRitem->World = rightSphereWorld;
    //    rightSphereRitem->ObjCBIndex = objCBIndex++;
    //    rightSphereRitem->Geo = mGeometries["shapeGeo"];
    //    rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    //    rightSphereRitem->IndexCount = rightSphereRitem->Geo->unit_mesh_geometries["sphere"].index_count;
    //    rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->unit_mesh_geometries["sphere"].start_index_location;
    //    rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->unit_mesh_geometries["sphere"].base_vertex_location;

    //    mAllRitems.push_back(leftCylRitem);
    //    mAllRitems.push_back(rightCylRitem);
    //    mAllRitems.push_back(leftSphereRitem);
    //    mAllRitems.push_back(rightSphereRitem);
    //}
    }

    m_initialized = true;
    return true;
}

//--------------------------------------------------------------------------------
//  Uninit the instance
//  インスタンスの終了処理
//  终了处理
//--------------------------------------------------------------------------------
void MainSystem::Uninitialize()
{
    RenderSystem::ShutDown();
    CameraSystem::ShutDown();
    EntitySystem::ShutDown();
    GameTimer::ShutDown();
}

//--------------------------------------------------------------------------------
//  Get the os' user language
//  OSの言語を取得
//  取得OS的语言
//--------------------------------------------------------------------------------
void MainSystem::GetSystemLanguage()
{
    LANGID os_language = GetUserDefaultUILanguage();

    switch (os_language)
    {
    case 0x0411:
        // Japanese
        m_current_language = Language::kJapanese;
        break;
    case 0x0004:
        // Chinese Symple
        m_current_language = Language::kChinese;
        break;
    default:
        // English
        m_current_language = Language::kEnglish;
        break;
    }
}

//--------------------------------------------------------------------------------
//  Initialize game window
//  Return：true when succeeded, else false
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  ウインドウ初期化処理
//  戻り値：成功したらtrue、失敗したらfalse
//ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
//  窗口初始化
//  返回值：成功则返回true、反之返回false
//--------------------------------------------------------------------------------
bool MainSystem::InitializeWindow()
{
    m_width = sc_default_width;
    m_height = sc_default_height;

    WNDCLASS window_class;
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = MainWndProc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = m_instance_handle;
    window_class.hIcon = LoadIcon(0, IDI_APPLICATION);
    window_class.hCursor = LoadCursor(0, IDC_ARROW);
    window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    window_class.lpszMenuName = 0;
    window_class.lpszClassName = kClassName;

    if (!RegisterClass(&window_class))
    {
        MessageBox(0, kFailedToRegisterClass[static_cast<uint32_t>(m_current_language)], 0, 0);
        return false;
    }

    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT rect = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    int width = static_cast<int>(rect.right - rect.left);
    int height = static_cast<int>(rect.bottom - rect.top);

    m_main_window_handle = CreateWindow
    (
        kClassName,
        kWindowName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        0,
        0,
        m_instance_handle,
        0
    );

    if (!m_main_window_handle)
    {
        MessageBox(0, kFailedToCreateWindow[static_cast<uint32_t>(m_current_language)], 0, 0);
        return false;
    }

    ShowWindow(m_main_window_handle, SW_SHOW);
    UpdateWindow(m_main_window_handle);
    return true;
}

//--------------------------------------------------------------------------------
//  Call the game systems' update function
//  ゲームシステムの更新
//  更新处理
//--------------------------------------------------------------------------------
void MainSystem::Update()
{
    UpdateWindowText();

    // test
    EntitySystem::Instance().PrepareRender();
}

//--------------------------------------------------------------------------------
//  Render the current scene
//  描画処理
//  渲染处理
//--------------------------------------------------------------------------------
void MainSystem::Render()
{
    RenderSystem::Instance().Render();
}

//--------------------------------------------------------------------------------
//  Update when WM_ACTIVATE is called
//  WM_ACTIVATEの時呼ばれる
//  WM_ACTIVATE时呼出
//--------------------------------------------------------------------------------
void MainSystem::OnWmActivate(WPARAM wparam)
{
    // We pause the game when the window is deactivated and unpause it 
    // when it becomes active. 
    m_paused = LOWORD(wparam) == WA_INACTIVE;
}

//--------------------------------------------------------------------------------
//  Update when WM_SIZE is called
//  WM_SIZEの時呼ばれる
//  WM_SIZE时呼出
//--------------------------------------------------------------------------------
void MainSystem::OnWmSize(WPARAM wparam, LPARAM lparam)
{
    // Save the new client area dimensions.
    m_width = LOWORD(lparam);
    m_height = HIWORD(lparam);
    
    if (m_initialized == false) return;

    if (wparam == SIZE_MINIMIZED)
    {
        m_paused = true;
        m_minimized = true;
        m_maximized = false;
    }
    else if (wparam == SIZE_MAXIMIZED)
    {
        m_paused = false;
        m_minimized = false;
        m_maximized = true;
        RenderSystem::Instance().OnResize();
    }
    else if (wparam == SIZE_RESTORED)
    {
        // Restoring from minimized state
        if (m_minimized)
        {
            m_paused = false;
            m_minimized = false;
            RenderSystem::Instance().OnResize();
        }

        // Restoring from maximized state
        else if (m_maximized)
        {
            m_paused = false;
            m_maximized = false;
            RenderSystem::Instance().OnResize();
        }
        else if (m_resizing)
        {
            // If user is dragging the resize bars, we do not resize 
            // the buffers here because as the user continuously 
            // drags the resize bars, a stream of WM_SIZE messages are
            // sent to the window, and it would be pointless (and slow)
            // to resize for each WM_SIZE message received from dragging
            // the resize bars.  So instead, we reset after the user is 
            // done resizing the window and releases the resize bars, which 
            // sends a WM_EXITSIZEMOVE message.
        }
        else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
        {
            RenderSystem::Instance().OnResize();
        }
    }
}

//--------------------------------------------------------------------------------
//  Update when WM_ENTERSIZEMOVE is called
//  WM_ENTERSIZEMOVEの時呼ばれる
//  WM_ENTERSIZEMOVE时呼出
//--------------------------------------------------------------------------------
void MainSystem::OnWmEnterSizeMove()
{
    m_paused = true;
    m_resizing = true;
}

//--------------------------------------------------------------------------------
//  Update when WM_EXITSIZEMOVE is called
//  WM_EXITSIZEMOVEの時呼ばれる
//  WM_EXITSIZEMOVE时呼出
//--------------------------------------------------------------------------------
void MainSystem::OnWmExitSizeMove()
{
    // Here we reset everything based on the new window dimensions.
    m_paused = false;
    m_resizing = false;
    if (m_initialized == false) return;
    RenderSystem::Instance().OnResize();
}

//--------------------------------------------------------------------------------
//  Update when WM_KEYUP is called
//  WM_KEYUPの時呼ばれる
//  WM_KEYUP时呼出
//--------------------------------------------------------------------------------
void MainSystem::OnWmKeyUp(WPARAM wparam, LPARAM lparam)
{
    if (wparam == VK_ESCAPE)
    {
        PostQuitMessage(0);
    }
    //else if ((int)wparam == VK_F2)
    //{
    //    if (render_system_) render_system_->SetMsaaState(!render_system_->GetMsaaState());
    //}
}

//--------------------------------------------------------------------------------
//  Update the window test
//  window text更新
//  更新窗口标题
//--------------------------------------------------------------------------------
void MainSystem::UpdateWindowText()
{
    // 全屏的时候不更新
    if (m_fullscreen_state) return;

    std::wstring text = kWindowName;
    text += L" FPS : " + std::to_wstring(GameTimer::Instance().Fps());
    SetWindowTextW(m_main_window_handle, text.c_str());
}