﻿//--------------------------------------------------------------------------------
//  base class of component
//  コンポーネントの基底クラス
//  组件的基类
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once

namespace KeepFortissimo
{
    //--------------------------------------------------------------------------------
    // 前方宣言
    //--------------------------------------------------------------------------------
    class Entity;

    class Component
    {
    public:
        //--------------------------------------------------------------------------------
        //  constructor
        //  コンストラクタ
        //  构造函数
        //--------------------------------------------------------------------------------
        Component(Entity& owner) : m_owner(owner) {}

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  析构函数
        //--------------------------------------------------------------------------------
        virtual ~Component() {}

        //--------------------------------------------------------------------------------
        //  初期化処理
        //  初始化
        //--------------------------------------------------------------------------------
        virtual void Initialize() {}

        //--------------------------------------------------------------------------------
        //  終了処理
        //  终止
        //--------------------------------------------------------------------------------
        virtual void Uninitialize() {}

    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        Component() = delete;
        Component(Component const&) = delete;
        void operator=(Component const&) = delete;

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        Entity& m_owner;
    };
}