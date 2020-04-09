//--------------------------------------------------------------------------------
//  exception function
//  例外関数
//  例外函数
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <Windows.h>
#include "../Systems/system_setting.h"
#include "kf_utilities.h"

namespace KeepFortissimo
{
    class Exception
    {
    public:
        //--------------------------------------------------------------------------------
        //  Constractor of Exception
        //  Arguments : error_code
        //              function_name
        //              file_name
        //              line_number
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  例外クラスのコンストラクタ
        //  引数 : error_code
        //         function_name
        //         file_name
        //         line_number
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  错误信息的构造函数
        //  参数 : error_code
        //         function_name
        //         file_name
        //         line_number
        //--------------------------------------------------------------------------------
        Exception(HRESULT error_code, const t_string& function_name, const t_string& file_name, int line_number);

        //--------------------------------------------------------------------------------
        //  Show exception in string
        //  Return：t_string
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  例外メッセージの表示
        //  戻り値：t_string
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  表示错误信息
        //  返回值：t_string
        //--------------------------------------------------------------------------------
        t_string ToString() const;

    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        Exception() = delete;
        void operator=(Exception const&) = delete;

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        HRESULT  m_error_code;
        t_string m_function_name;
        t_string m_file_name;
        int      m_line_number;
    };

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                                        \
{                                                                               \
    HRESULT hresult = (x);                                                      \
    const t_string& file_name = Utility::AnsiToString(__FILE__);                         \
    if(FAILED(hresult)) { throw Exception(hresult, L#x, file_name, __LINE__); } \
}
#endif
}