//--------------------------------------------------------------------------------
//  start up the main system from windows
//  アプリ起動用cpp
//  用于从windows启动程序
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#include "Utilities\kf_utilities.h"
#include "Utilities\kf_labels.h"
#include "Systems\main_system.h"
using namespace KeepFortissimo;

int WINAPI WinMain(HINSTANCE instance_handle, HINSTANCE previous_instance_handle, PSTR command_line, int show_command)
{
#if defined(DEBUG) | defined(_DEBUG)
    // Enable run-time memory check for debug builds.
    // 开启运行时的内存检测，方便监督内存泄露的情况
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        if (!MainSystem::StartUp(instance_handle))
        {
            MainSystem::ShutDown();
            return 0;
        }

        int result = MainSystem::Instance().Run();
        MainSystem::ShutDown();
        return result;
    }
    catch (Exception& exception)
    {
        MessageBox(nullptr, exception.ToString().c_str(), kFailedToStartUpGameSystem[static_cast<u32>(Language::kEnglish)], MB_OK);
        return 0;
    }
}