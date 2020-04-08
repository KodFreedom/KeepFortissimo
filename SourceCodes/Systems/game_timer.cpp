//--------------------------------------------------------------------------------
//  GameTimer
//  時間管理システム
//  游戏内时间管理系统
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "game_timer.h"
#include "main_system.h"
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  SetFpsLimit
//--------------------------------------------------------------------------------
void GameTimer::SetFpsLimit(const uint32_t fps_limit)
{
    m_fps_limit = fps_limit;

    if (m_fps_limit == 0)
    {
        m_time_interval = FLT_MIN;
    }
    else
    {
        m_time_interval = 1.0f / static_cast<float>(m_fps_limit);
    }
}

//--------------------------------------------------------------------------------
//  Tick
//--------------------------------------------------------------------------------
void GameTimer::Tick()
{
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_current_time);

    m_delta_time = static_cast<float>(m_current_time.QuadPart - m_exec_last_time.QuadPart)
        / static_cast<float>(m_frequency.QuadPart);

    m_scaled_delta_time = m_delta_time * m_time_scale
        * static_cast<float>(MainSystem::Instance().Paused() ^ 1);

    m_fps = m_delta_time > 0.0f ? 1.0f / m_delta_time : 0.0f;
}

//--------------------------------------------------------------------------------
//  CanUpdateFrame
//--------------------------------------------------------------------------------
bool GameTimer::CanUpdateFrame(void)
{
    if (MainSystem::Instance().Paused())
    {
        return false;
    }
    else if (m_delta_time >= m_time_interval)
    {
        m_exec_last_time = m_current_time;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//  GameTimer
//--------------------------------------------------------------------------------
GameTimer::GameTimer()
    : Singleton<GameTimer>()
{
    memset(&m_frequency, 0x00, sizeof m_frequency);
    memset(&m_current_time, 0x00, sizeof m_current_time);
    memset(&m_exec_last_time, 0x00, sizeof m_exec_last_time);
    memset(&m_fps_last_time, 0x00, sizeof m_fps_last_time);
}

//--------------------------------------------------------------------------------
//  ~GameTimer
//--------------------------------------------------------------------------------
GameTimer::~GameTimer()
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
bool GameTimer::Initialize()
{
    m_time_scale = 1.0f;
    QueryPerformanceCounter(&m_exec_last_time);
    m_fps_last_time = m_exec_last_time;
    SetFpsLimit(sc_default_fps_limit);
    return true;
}