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
#include <DirectXColors.h>
#include <list>
#include "../../Utilities/singleton.h"
#include "render_define.h"

namespace KeepFortissimo
{
    //--------------------------------------------------------------------------------
    // 前方宣言
    //--------------------------------------------------------------------------------
    class Renderer;

    //--------------------------------------------------------------------------------
    // enum
    //--------------------------------------------------------------------------------
    enum class RenderApiType : uint32_t
    {
        kInvalid,
        kDirectX12,
    };

    class RenderSystem : public Singleton<RenderSystem>
    {
        friend class Singleton<RenderSystem>;

    public:
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
        static bool StartUp() = delete;
        static bool StartUp(const RenderApiType type);

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
        virtual void Render();

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
        virtual void OnResize() {}

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
        bool GetMsaaEnable() const { return m_msaa_enable; }

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

        //--------------------------------------------------------------------------------
        //  Get the render api type
        //  Arguments :
        //  Return：RenderApiType
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  レンダーApiタイプの取得
        //  引数 :
        //  戻り値：RenderApiType
        //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
        //  取得渲染Api类型
        //  参数 :
        //  返回值：RenderApiType
        //--------------------------------------------------------------------------------
        RenderApiType GetRenderApiType() const { return m_api_type; }

        //--------------------------------------------------------------------------------
        //  Add renderer
        //--------------------------------------------------------------------------------
        void Add(Renderer* renderer);

    protected:
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
        //  初期化処理
        //  初始化
        //--------------------------------------------------------------------------------
        virtual bool Initialize() override { return true; };

        //--------------------------------------------------------------------------------
        //  Uninit the instance
        //  インスタンスの終了処理
        //  终了处理
        //--------------------------------------------------------------------------------
        virtual void Uninitialize() override {};

        //--------------------------------------------------------------------------------
        //  Clean up
        //  クリーンアップ
        //  善后
        //--------------------------------------------------------------------------------
        void CleanUp();

        //--------------------------------------------------------------------------------
        //  variable / 変数 / 变量
        //--------------------------------------------------------------------------------
        bool                 m_msaa_enable = false;
        uint32_t             m_msaa_quality = 0;
        DirectX::XMVECTORF32 m_background_color = DirectX::Colors::Yellow;
        RenderApiType        m_api_type = RenderApiType::kInvalid;
        std::list<Renderer*> m_renderers[static_cast<uint32_t>(RenderPriority::kMax)] = {};
    };
}