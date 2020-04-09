//--------------------------------------------------------------------------------
//  define of component
//  コンポーネントの定義
//  组件的定义
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <stdint.h>
#include "../Utilities/type_name.h"
//
// TODO : 生成を自動化する
// 

namespace KeepFortissimo
{
    //--------------------------------------------------------------------------------
    // enum / 列挙型
    //--------------------------------------------------------------------------------
    //enum class ComponentId : uint32_t 
    //{
    //    kComponent = 0,
    //    kTransform,
    //    kMax
    //};

    //--------------------------------------------------------------------------------
    // 前方宣言
    //--------------------------------------------------------------------------------
    class Component;
    class Transform;

    // component generatorを作る
    // function<void*()>[kMax] ConstructMap;
    // ConstructMap[kTransform] = []() { return new Transform(); };
}

//--------------------------------------------------------------------------------
// define
//--------------------------------------------------------------------------------
ENABLE_TYPENAME(Component);
ENABLE_TYPENAME(Transform);