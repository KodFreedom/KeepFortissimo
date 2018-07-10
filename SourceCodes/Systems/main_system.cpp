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
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//
//  Public
//
//--------------------------------------------------------------------------------
bool MainSystem::StartUp(HINSTANCE instance_handle)
{
    if (instance_ != nullptr) return true;
    MY_NEW MainSystem(instance_handle);
    return instance_->Initialize();
}

int MainSystem::Run()
{
    return 0;
}

//--------------------------------------------------------------------------------
//
//  Private
//
//--------------------------------------------------------------------------------
MainSystem::MainSystem(HINSTANCE instance_handle)
    : Singleton<MainSystem>()
    , instance_handle_(instance_handle)
    , current_language_(kEnglish)
{

}

MainSystem::~MainSystem()
{

}

bool MainSystem::Initialize()
{
    GetSystemLanguage();
    
    if (!GameTimer::StartUp())
    {
        return false;
    }

    return true;
}

void MainSystem::Uninitialize()
{

}

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