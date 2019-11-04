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
    class Utility
    {
    public:
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
        inline static t_string AnsiToString(const std::string& value)
        {
#ifdef UNICODE
            WCHAR buffer[512];
            MultiByteToWideChar(CP_ACP, 0, value.c_str(), -1, buffer, 512);
            return t_string(buffer);
#else
            return value;
#endif // !UNICODE
        }

        //--------------------------------------------------------------------------------
        //  Calculate constant buffer byte size to multiple of 256
        //  Arguments : value : byte_size
        //  Return：u32
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  コンスタントバッファサイズを256の倍数に変更
        //  引数 : value : byte_size
        //  戻り値：u32
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  将常量缓冲区的大小转换为256的倍数
        //  参数 : value : byte_size
        //  返回值：u32
        //--------------------------------------------------------------------------------
        inline static u32 CalculateConstantBufferByteSize(u32 byte_size)
        {
            // 常量缓冲区的大小必须是硬件最小分配空间的整数倍（通常是256b）
            // 为此要将其凑整为满足需求的最小的256的整数倍。我们现在通过输入值bytesize加上255，
            // 在屏蔽求和结果的低2字节（及计算结果中小于256的数据部分）来实现这一点
            return (byte_size + 255) & ~255;
        }

    private:
        Utility() = delete;
        Utility(const Utility& rhs) = delete;
        Utility& operator=(const Utility& rhs) = delete;
    };
}