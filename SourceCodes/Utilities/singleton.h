﻿//--------------------------------------------------------------------------------
//  singleton template class
//  シングルトーンのテンプレートクラス
//  单例的基类
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include "../Systems/system_setting.h"

namespace KeepFortissimo
{
    template <typename T>
    class Singleton
    {
    public:
        //--------------------------------------------------------------------------------
        //  Get the instance
        //  ＊Warning＊ Must call StartUp before call this method
        //  Arguments :
        //  Return：reference type instance
        //--------------------------------------------------------------------------------
        //  インスタンスの取得
        //  ＊警告＊必ずStartUpを読んでからこの関数を呼ぶ
        //  引数 :
        //  戻り値：インスタンスの参照型
        //--------------------------------------------------------------------------------
        //  取得实体
        //  ＊警告＊实行StartUp之后再实行此函数
        //  参数 :
        //  返回值：实体的参照型
        //--------------------------------------------------------------------------------
        static T& Instance()
        {
            assert(m_instance != nullptr);
            return *m_instance;
        }

        //--------------------------------------------------------------------------------
        //  Create the instance and initialize it
        //  Arguments :
        //  Return：true when succeeded, else false
        //--------------------------------------------------------------------------------
        //  インスタンスの生成と初期化処理
        //  引数 :
        //  戻り値：成功したらtrue、失敗したらfalse
        //--------------------------------------------------------------------------------
        //  生成实体并初始化
        //  参数 :
        //  返回值：成功则返回true、反之返回false
        //--------------------------------------------------------------------------------
        static bool StartUp()
        {
            if (m_instance != nullptr) return true;
            MY_NEW T();
            return m_instance->Initialize();
        }

        //--------------------------------------------------------------------------------
        //  Uninit the instance and delete it
        //  Arguments :
        //  Return：
        //--------------------------------------------------------------------------------
        //  インスタンスの終了と破棄処理
        //  引数 :
        //  戻り値：
        //--------------------------------------------------------------------------------
        //  终了并移除实体
        //  参数 :
        //  返回值：
        //--------------------------------------------------------------------------------
        static void ShutDown()
        {
            SAFE_UNINIT(m_instance);
        }

    protected:
        //--------------------------------------------------------------------------------
        //  constructor
        //  コンストラクタ
        //  构造函数
        //--------------------------------------------------------------------------------
        Singleton()
        {
            assert(m_instance == nullptr);
            m_instance = static_cast<T*>(this);
        }

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  析构函数
        //--------------------------------------------------------------------------------
        virtual ~Singleton()
        {
        }

        //--------------------------------------------------------------------------------
        //  initialize the instance
        //  Arguments :
        //  Return：true when succeeded, else false
        //--------------------------------------------------------------------------------
        //  初期化処理
        //  引数 :
        //  戻り値：成功したらtrue、失敗したらfalse
        //--------------------------------------------------------------------------------
        //  初始化
        //  参数 :
        //  返回值：成功则返回true、反之返回false
        //--------------------------------------------------------------------------------
        virtual bool Initialize() = 0;

        //--------------------------------------------------------------------------------
        //  Uninit the instance
        //  Arguments :
        //  Return：
        //--------------------------------------------------------------------------------
        //  インスタンスの終了処理
        //  引数 :
        //  戻り値：
        //--------------------------------------------------------------------------------
        //  终了处理
        //  参数 :
        //  返回值：
        //--------------------------------------------------------------------------------
        virtual void Uninitialize() = 0;

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        static T* m_instance;

    private:
        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        Singleton(Singleton const&) = delete;
        void operator=(Singleton const&) = delete;
    };

    //--------------------------------------------------------------------------------
    //  initialize the static variable
    //  静的メンバー変数の初期化
    //  静态成员变量的初始化
    //--------------------------------------------------------------------------------
    template<typename T>
    T* Singleton<T>::m_instance = nullptr;
}