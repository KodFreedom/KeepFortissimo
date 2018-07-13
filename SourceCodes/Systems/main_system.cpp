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
#include "../Utilities/kf_labels.h"
#include "render_system_directx12.h"
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
    if (instance_ != nullptr) return true;
    MY_NEW MainSystem(instance_handle);
    return instance_->Initialize();
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
    UnregisterClass(kClassName, instance_handle_);

    return (int)msg.wParam;
}

//--------------------------------------------------------------------------------
//  MsgProc
//--------------------------------------------------------------------------------
LRESULT MainSystem::MsgProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
        // WM_ACTIVATE is sent when the window is activated or deactivated.  
        // We pause the game when the window is deactivated and unpause it 
        // when it becomes active.  
    case WM_ACTIVATE:
        if (LOWORD(wparam) == WA_INACTIVE)
        {
            paused_ = true;
            GameTimer::Instance().SetTimeScale(0.0f);
        }
        else
        {
            paused_ = false;
            GameTimer::Instance().SetTimeScale(1.0f);
        }
        return 0;

        // WM_SIZE is sent when the user resizes the window.  
    case WM_SIZE:
        // Save the new client area dimensions.
        width_ = LOWORD(lparam);
        height_ = HIWORD(lparam);
        //if (render_system_)
        //{
        //    if (wparam == SIZE_MINIMIZED)
        //    {
        //        paused_ = true;
        //        minimized_ = true;
        //        maximized_ = false;
        //    }
        //    else if (wparam == SIZE_MAXIMIZED)
        //    {
        //        paused_ = false;
        //        minimized_ = false;
        //        maximized_ = true;
        //        render_system_->OnResize();
        //    }
        //    else if (wparam == SIZE_RESTORED)
        //    {
        //        // Restoring from minimized state?
        //        if (minimized_)
        //        {
        //            paused_ = false;
        //            minimized_ = false;
        //            render_system_->OnResize();
        //        }

        //        // Restoring from maximized state?
        //        else if (maximized_)
        //        {
        //            paused_ = false;
        //            maximized_ = false;
        //            render_system_->OnResize();
        //        }
        //        else if (resizing_)
        //        {
        //            // If user is dragging the resize bars, we do not resize 
        //            // the buffers here because as the user continuously 
        //            // drags the resize bars, a stream of WM_SIZE messages are
        //            // sent to the window, and it would be pointless (and slow)
        //            // to resize for each WM_SIZE message received from dragging
        //            // the resize bars.  So instead, we reset after the user is 
        //            // done resizing the window and releases the resize bars, which 
        //            // sends a WM_EXITSIZEMOVE message.
        //        }
        //        else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
        //        {
        //            render_system_->OnResize();
        //        }
        //    }
        //}
        return 0;

        // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
    case WM_ENTERSIZEMOVE:
        paused_ = true;
        resizing_ = true;
        GameTimer::Instance().SetTimeScale(0.0f);
        return 0;

        // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
        // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
        paused_ = false;
        resizing_ = false;
        GameTimer::Instance().SetTimeScale(1.0f);
        //if (render_system_) render_system_->OnResize();
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
        if (wparam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
        //else if ((int)wparam == VK_F2)
        //{
        //    if (render_system_) render_system_->SetMsaaState(!render_system_->GetMsaaState());
        //}

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
    , instance_handle_(instance_handle)
    , main_window_handle_(nullptr)
    , paused_(false)
    , minimized_(false)
    , maximized_(false)
    , resizing_(false)
    , fullscreen_state_(false)
    , width_(kDefaultWidth)
    , height_(kDefaultHeight)
    , current_language_(kEnglish)
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

    if (!RenderSystemDirectX12::StartUp())
    {
        return false;
    }

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
        current_language_ = kJapanese;
        break;
    case 0x0004:
        // Chinese Symple
        current_language_ = kChinese;
        break;
    default:
        // English
        current_language_ = kEnglish;
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
    WNDCLASS window_class;
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = MainWndProc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = instance_handle_;
    window_class.hIcon = LoadIcon(0, IDI_APPLICATION);
    window_class.hCursor = LoadCursor(0, IDC_ARROW);
    window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(NULL_BRUSH));
    window_class.lpszMenuName = 0;
    window_class.lpszClassName = kClassName;

    if (!RegisterClass(&window_class))
    {
        MessageBox(0, kFailedToRegisterClass[current_language_], 0, 0);
        return false;
    }

    // Compute window rectangle dimensions based on requested client area dimensions.
    RECT rect = { 0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_) };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);
    int width = static_cast<int>(rect.right - rect.left);
    int height = static_cast<int>(rect.bottom - rect.top);

    main_window_handle_ = CreateWindow
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
        instance_handle_,
        0
    );

    if (!main_window_handle_)
    {
        MessageBox(0, kFailedToCreateWindow[current_language_], 0, 0);
        return false;
    }

    ShowWindow(main_window_handle_, SW_SHOW);
    UpdateWindow(main_window_handle_);
    return true;
}

//--------------------------------------------------------------------------------
//  Call the game systems' update function
//  ゲームシステムの更新
//  更新处理
//--------------------------------------------------------------------------------
void MainSystem::Update()
{

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