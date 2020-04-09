//--------------------------------------------------------------------------------
//  exception function
//  ó·äOä÷êî
//  ó·äOîüêî
//
//  Autor  : èô ï∂û^(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#include "exception.h"
#include <comdef.h>
using namespace KeepFortissimo;

//--------------------------------------------------------------------------------
//  Exception
//--------------------------------------------------------------------------------
Exception::Exception(HRESULT error_code, const t_string& function_name, const t_string& file_name, int line_number)
    : m_error_code(error_code)
    , m_function_name(function_name)
    , m_file_name(file_name)
    , m_line_number(line_number)
{
}

//--------------------------------------------------------------------------------
//  ToString
//--------------------------------------------------------------------------------
t_string Exception::ToString() const
{
    // Get the string description of the error code.
    _com_error error(m_error_code);
    t_string message = error.ErrorMessage();

#ifdef UNICODE
    return m_function_name + L" failed in " + m_file_name + L"; line " + std::to_wstring(m_line_number) + L"; error: " + message;
#else
    return m_function_name + " failed in " + m_file_name + "; line " + std::to_string(m_line_number) + "; error: " + message;
#endif // !UNICODE
}