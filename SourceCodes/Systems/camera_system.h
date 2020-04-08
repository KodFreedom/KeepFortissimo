﻿//--------------------------------------------------------------------------------
//  camera system
//  カメラシステム
//  摄像机系统
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include "../Utilities/singleton.h"

namespace KeepFortissimo
{
    //--------------------------------------------------------------------------------
    // 前方宣言
    //--------------------------------------------------------------------------------
    class Camera;

    class CameraSystem : public Singleton<CameraSystem>
    {
        friend class Singleton<CameraSystem>;

    public:

    private:
        //--------------------------------------------------------------------------------
        //  constructor
        //  コンストラクタ
        //  构造函数
        //--------------------------------------------------------------------------------
        CameraSystem() {}

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  析构函数
        //--------------------------------------------------------------------------------
        ~CameraSystem() {}

        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        CameraSystem(CameraSystem const&) = delete;
        void operator=(CameraSystem const&) = delete;

        //--------------------------------------------------------------------------------
        //  initialize the instance
        //  初期化処理
        //  初始化
        //--------------------------------------------------------------------------------
        bool Initialize() override { return true; }

        //--------------------------------------------------------------------------------
        //  Uninit the instance
        //  インスタンスの終了処理
        //  终了处理
        //--------------------------------------------------------------------------------
        void Uninitialize() override {}
    };
}