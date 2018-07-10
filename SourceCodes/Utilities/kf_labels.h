//--------------------------------------------------------------------------------
//  labels
//  ラベル
//  文本
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include "../Systems/system_setting.h"

namespace KeepFortissimo
{
    enum Language
    {
        kEnglish = 0,
        kChinese,
        kJapanese,
        kLanguageMax
    };

#ifdef UNICODE
    static const wchar_t* kFailedToStartUpGameSystem[kLanguageMax] =
    {
        L"Failed to start up game system",
        L"游戏系统启动失败"
        L"ゲームシステムの起動に失敗"
    };
#else
    static const char* kFailedToStartUpGameSystem[kLanguageMax] =
    {
        "Failed to start up game system",
        u8"游戏系统启动失败"
        u8"ゲームシステムの起動に失敗"
    };
#endif // !UNICODE

}