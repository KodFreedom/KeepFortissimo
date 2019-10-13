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

    //--------------------------------------------------------------------------------
    //  Change ansi to string(multibyte)/wstring(unicode)
    //  Arguments : value : string for change
    //  Return：t_string
    //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
    //  ansiをstring(multibyte)/wstring(unicode)に変換する
    //  引数 : value : 転換する内容
    //  戻り値：t_string
    //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
    //  将ansi转换为string(multibyte)/wstring(unicode)
    //  参数 : value : 转换对象
    //  返回值：t_string
    //--------------------------------------------------------------------------------
    inline t_string AnsiToString(const std::string& value)
    {
#ifdef UNICODE
        WCHAR buffer[512];
        MultiByteToWideChar(CP_ACP, 0, value.c_str(), -1, buffer, 512);
        return t_string(buffer);
#else
        return value;
#endif // !UNICODE
    }

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                                        \
{                                                                               \
    HRESULT hresult = (x);                                                      \
    const t_string& file_name = AnsiToString(__FILE__);                         \
    if(FAILED(hresult)) { throw Exception(hresult, L#x, file_name, __LINE__); } \
}
#endif
}