//--------------------------------------------------------------------------------
//  type name
//  
//  
//
//  Autor  : �� �Ľ�(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once

namespace KeepFortissimo
{
    // default
    template <typename T>
    struct TypeName
    {
        static const char* Get()
        {
            return typeid(T).name();
        }
    };
}

// specialization 
#define ENABLE_TYPENAME(A) namespace KeepFortissimo{ template<> struct TypeName<A> { static const char *Get() { return #A; }};}