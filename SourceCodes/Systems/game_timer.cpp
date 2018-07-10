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
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
void GameTimer::SetFpsLimit(const UINT fps_limit)
{
    fps_limit_ = fps_limit;

    if (fps_limit_ == 0)
    {
        time_interval_ = FLT_MIN;
    }
    else
    {
        time_interval_ = 1.0f / static_cast<float>(fps_limit_);
    }
}

void GameTimer::Tick()
{
    QueryPerformanceFrequency(&frequency_);
    QueryPerformanceCounter(&current_time_);

    delta_time_ = static_cast<float>(current_time_.QuadPart - exec_last_time_.QuadPart)
        / static_cast<float>(frequency_.QuadPart);

    scaled_delta_time_ = delta_time_ * time_scale_;
}

bool GameTimer::CanUpdateFrame(void)
{
    if (delta_time_ >= time_interval_)
    {
        exec_last_time_ = current_time_;
        return true;
    }
    return false;
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
GameTimer::GameTimer()
    : Singleton<GameTimer>()
    , delta_time_(0.0f)
    , time_scale_(1.0f)
    , scaled_delta_time_(0.0f)
    , time_interval_(0.0f)
    , fps_limit_(0)
{
    memset(&frequency_, 0x00, sizeof frequency_);
    memset(&current_time_, 0x00, sizeof current_time_);
    memset(&exec_last_time_, 0x00, sizeof exec_last_time_);
    memset(&fps_last_time_, 0x00, sizeof fps_last_time_);
}

GameTimer::~GameTimer()
{

}

bool GameTimer::Initialize()
{
    QueryPerformanceCounter(&exec_last_time_);
    fps_last_time_ = exec_last_time_;
    SetFpsLimit(kDefaultFpsLimit);
    return true;
}