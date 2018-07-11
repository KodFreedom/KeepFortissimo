﻿//--------------------------------------------------------------------------------
//  base class of render system
//  描画システムのベース
//  渲染系统的基类
//
//  Autor  : 徐 文杰(Wenjie Xu)
//  Github : kodfreedom
//  Email  : kodfreedom@gmail.com
//--------------------------------------------------------------------------------
#pragma once
#include <Windows.h>
#include "../Utilities/singleton.h"
#include "../Libraries/DirectXMath/DirectXColors.h"

namespace KeepFortissimo
{
    enum RenderApiType
    {
        kDirectX12,
    };

    class RenderSystem : public Singleton<RenderSystem>
    {
        friend class Singleton<RenderSystem>;

    public:
        //--------------------------------------------------------------------------------
        //  Render all registered components
        //  Arguments :
        //  Return：
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  登録したコンポネントを描画する
        //  引数 :
        //  戻り値：
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  将注册过的组件进行渲染
        //  参数 :
        //  返回值：
        //--------------------------------------------------------------------------------
        void Render();

        //--------------------------------------------------------------------------------
        //  Called when changed window size
        //  Arguments :
        //  Return：
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  ウインドウサイズ変わった時に呼ばれる
        //  引数 :
        //  戻り値：
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  窗口尺寸改变时被呼出
        //  参数 :
        //  返回值：
        //--------------------------------------------------------------------------------
        virtual void OnResize() = 0;

        //--------------------------------------------------------------------------------
        //  Get if enabled MSAA
        //  Arguments :
        //  Return：true when enabled, else false
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  初期化処理
        //  引数 :
        //  戻り値：成功したらtrue、失敗したらfalse
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  取得是否使用MSAA
        //  参数 :
        //  返回值：使用则返回true、反之返回false
        //--------------------------------------------------------------------------------
        bool GetMsaaEnable() const { return msaa_enable_; }

        //--------------------------------------------------------------------------------
        //  Set if enabled MSAA
        //  Arguments : bool value
        //  Return：
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  MSAAの適用の設定
        //  引数 : bool value
        //  戻り値：
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  设定是否使用MSAA
        //  参数 : bool value
        //  返回值：
        //--------------------------------------------------------------------------------
        void SetMsaaEnable(bool value);

    private:
        //--------------------------------------------------------------------------------
        //  constructor
        //  コンストラクタ
        //  构造函数
        //--------------------------------------------------------------------------------
        RenderSystem(const RenderApiType type);

        //--------------------------------------------------------------------------------
        //  destructor
        //  デストラクタ
        //  析构函数
        //--------------------------------------------------------------------------------
        virtual ~RenderSystem();

        //--------------------------------------------------------------------------------
        //  delete the copy constructor and operator
        //  コピーコンストラクタとオペレーターの削除
        //  删除复制用构造函数与等号
        //--------------------------------------------------------------------------------
        RenderSystem() = delete;
        RenderSystem(RenderSystem const&) = delete;
        void operator=(RenderSystem const&) = delete;

        //--------------------------------------------------------------------------------
        //  initialize the instance
        //  Arguments :
        //  Return：true when succeeded, else false
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  初期化処理
        //  引数 :
        //  戻り値：成功したらtrue、失敗したらfalse
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  初始化
        //  参数 :
        //  返回值：成功则返回true、反之返回false
        //--------------------------------------------------------------------------------
        virtual bool Initialize() override = 0;

        //--------------------------------------------------------------------------------
        //  Uninit the instance
        //  Arguments :
        //  Return：
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  インスタンスの終了処理
        //  引数 :
        //  戻り値：
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  终了处理
        //  参数 :
        //  返回值：
        //--------------------------------------------------------------------------------
        virtual void Uninitialize() override = 0;

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        bool              msaa_enable_;
        UINT              msaa_quality_;
        DirectX::XMVECTOR background_color_;
        RenderApiType     api_type_;
    };
}