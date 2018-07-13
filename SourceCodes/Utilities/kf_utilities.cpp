//--------------------------------------------------------------------------------
//  utility functions
//  便利関数
//  便利函数
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include <comdef.h>
#include "kf_utilities.h"
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//  Exception
//--------------------------------------------------------------------------------
Exception::Exception(HRESULT error_code, const String& function_name, const String& file_name, int line_number)
    : error_code_(error_code)
    , function_name_(function_name)
    , file_name_(file_name)
    , line_number_(line_number)
{
}

//--------------------------------------------------------------------------------
//  ToString
//--------------------------------------------------------------------------------
String Exception::ToString() const
{
    // Get the string description of the error code.
    _com_error error(error_code_);
    String message = error.ErrorMessage();

#ifdef UNICODE
    return function_name_ + L" failed in " + file_name_ + L"; line " + std::to_wstring(line_number_) + L"; error: " + message;
#else
    return function_name_ + " failed in " + file_name_ + "; line " + std::to_string(line_number_) + "; error: " + message;
#endif // !UNICODE
}