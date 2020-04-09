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
#include <stdint.h>

namespace KeepFortissimo
{
    enum class Language : uint32_t
    {
        kEnglish = 0,
        kChinese,
        kJapanese,
        kLanguageMax
    };

#ifdef UNICODE
    static const wchar_t* kFailedToStartUpGameSystem[static_cast<uint32_t>(Language::kLanguageMax)] =
    {
        L"Failed to start up game system",
        L"游戏系统启动失败"
        L"ゲームシステムの起動に失敗"
    };

    static const wchar_t* kClassName = L"KeepFortissimo";

    static const wchar_t* kWindowName = L"KeepFortissimo";

    static const wchar_t* kFailedToRegisterClass[static_cast<uint32_t>(Language::kLanguageMax)] =
    {
        L"RegisterClass returned false ! \n MainSystem/InitializeWindow",
        L"RegisterClass函数返回false ! \n MainSystem/InitializeWindow"
        L"RegisterClassが失敗した ! \n MainSystem/InitializeWindow"
    };

    static const wchar_t* kFailedToCreateWindow[static_cast<uint32_t>(Language::kLanguageMax)] =
    {
        L"CreateWindow returned null ! \n MainSystem/InitializeWindow",
        L"CreateWindow函数返回null ! \n MainSystem/InitializeWindow"
        L"CreateWindowが失敗した ! \n MainSystem/InitializeWindow"
    };
#else
    static const char* kFailedToStartUpGameSystem[static_cast<uint32_t>(Language::kLanguageMax)] =
    {
        "Failed to start up game system",
        u8"游戏系统启动失败"
        u8"ゲームシステムの起動に失敗"
    };

    static const char* kClassName = "KeepFortissimo";

    static const char* kWindowName = "KeepFortissimo";

    static const char* kFailedToRegisterClass[static_cast<uint32_t>(Language::kLanguageMax)] =
    {
        "RegisterClass returned false ! \n MainSystem/InitializeWindow",
        u8"RegisterClass函数返回false ! \n MainSystem/InitializeWindow"
        u8"RegisterClassが失敗した ! \n MainSystem/InitializeWindow"
    };

    static const char* kFailedToCreateWindow[static_cast<uint32_t>(Language::kLanguageMax)] =
    {
        "CreateWindow returned null ! \n MainSystem/InitializeWindow",
        u8"CreateWindow函数返回null ! \n MainSystem/InitializeWindow"
        u8"CreateWindowが失敗した ! \n MainSystem/InitializeWindow"
    };
#endif // !UNICODE
}