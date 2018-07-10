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
    return true;
}

void MainSystem::Uninitialize()
{

}