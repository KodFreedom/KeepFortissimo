//--------------------------------------------------------------------------------
//  utility functions
//  便利関数
//  便利函数
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <Windows.h>
#include "../Systems/system_setting.h"

namespace KeepFortissimo
{
    class Exception
    {
    public:
        Exception() = default;
        Exception(HRESULT hresult, const String& function_name, const String& file_name, int line_number) {}

        String  ToString() const { return String(); }

        HRESULT error_code_ = S_OK;
        String  function_name_;
        String  file_name_;
        int     line_number_ = -1;
    };
}