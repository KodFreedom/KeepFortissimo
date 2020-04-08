//--------------------------------------------------------------------------------
//  system setting
//  システム設定
//  系统设定
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once

//--------------------------------------------------------------------------------
//  common include files
//  共通インクルードファイル
//  共通头文件
//--------------------------------------------------------------------------------
#include <assert.h>
#include <string>

//--------------------------------------------------------------------------------
//  common define/typedef
//  共通ディファイン
//  共通定义
//--------------------------------------------------------------------------------
#ifdef UNICODE
typedef std::wstring t_string;
#else
typedef std::string t_string;
#endif // !UNICODE

#define MY_NEW new
#define MY_DELETE delete

#define SAFE_RELEASE(p) if(p){ p->Release(); p = nullptr; }
#define SAFE_UNINIT(p)  if(p){ p->Uninitialize(); MY_DELETE p; p = nullptr; }
#define SAFE_DELETE(p)  if(p){ MY_DELETE p; p = nullptr; }