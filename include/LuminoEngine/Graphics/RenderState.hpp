﻿
#pragma once 

namespace ln {

/** ブレンディングの演算方法 */
enum class BlendOp : uint8_t
{
	Add,						/**< 転送元に転送先を加算する。*/
	Subtract,					/**< 転送元から転送先を減算する。*/
	ReverseSubtract,			/**< 転送先から転送元を減算する。*/
	Min,						/**< 転送先と転送元から小さいほうを使用する。*/
	Max,						/**< 転送先と転送元から大きいほうを使用する。*/
};

/** ブレンディングの係数 */
enum class BlendFactor : uint8_t
{
	Zero,						/**< ブレンディング係数は、(0, 0, 0, 0) */
	One,						/**< ブレンディング係数は、(1, 1, 1, 1) */
	SourceColor,				/**< ブレンディング係数は、(Rs, Gs, Bs, As) */
	InverseSourceColor,			/**< ブレンディング係数は、(1-Rs, 1-Gs, 1-Bs, 1-As) */
	SourceAlpha,				/**< ブレンディング係数は、(As, As, As, As) */
	InverseSourceAlpha,			/**< ブレンディング係数は、(1-As, 1-As, 1-As, 1-As) */
	DestinationColor,			/**< ブレンディング係数は、(Rd, Gd, Bd, Ad) */
	InverseDestinationColor,	/**< ブレンディング係数は、(1-Rd, 1-Gd, 1-Bd, 1-Ad) */
	DestinationAlpha,			/**< ブレンディング係数は、(Ad, Ad, Ad, Ad) */
	InverseDestinationAlpha,	/**< ブレンディング係数は、(1-Ad, 1-Ad, 1-Ad, 1-Ad) */
};

/** 塗りつぶし方法 */
enum class FillMode : uint8_t
{
    /** 面を塗りつぶす */
    Solid,

    /** ワイヤーフレーム */
    Wireframe,
};

/** カリング方法 */
enum class CullingMode : uint8_t
{
	/** 両面を描画します。 */
	None,

	/** 前向きの面を描画しません。 */
	Front,

	/** 後ろ向きの面を描画しません。 */
	Back,
};

/// 比較関数の定数
enum class ComparisonFunc : uint8_t
{
	Never,					///< 常に失敗します。
	Less,						///< (新しいピクセル値 < 現在のピクセル値) 新しいピクセル値が、現在のピクセル値未満の場合に、新しいピクセル値を採用します。
	LessEqual,					///< (新しいピクセル値 <= 現在のピクセル値) 新しいピクセル値が、現在のピクセル値以下の場合に、新しいピクセル値を採用します。
	Greater,					///< (新しいピクセル値 > 現在のピクセル値) 新しいピクセル値が、現在のピクセル値を超える場合に、新しいピクセル値を採用します。
	GreaterEqual,				///< (新しいピクセル値 >= 現在のピクセル値) 新しいピクセル値が、現在のピクセル値以上の場合に、新しいピクセル値を採用します。
	Equal,						///< (新しいピクセル値 == 現在のピクセル値) 新しいピクセル値が、現在のピクセル値と等しい場合に、新しいピクセル値を採用します。
	NotEqual,					///< (新しいピクセル値 != 現在のピクセル値) 新しいピクセル値が、現在のピクセル値と等しくない場合に、新しいピクセル値を採用します。
	Always,						///< 常に成功します。
};

/// ステンシルテストの結果に応じたステンシル処理方法です。
enum class StencilOp
{
	Keep,					///< 既存のステンシル データを保持します。(何もしません)
	Replace,					///< ステンシルデータをステンシル参照値に設定します。
};

///** レンダリングステート */
//struct RenderStateDesc
//{
//	/** アルファブレンドの有無 */
//	bool alphaBlendEnabled;
//
//	/** ブレンディングの演算方法 */
//	BlendOp blendOp;
//
//	/** ブレンディングの係数 */
//	BlendFactor sourceBlend;
//
//	/** ブレンディングの係数 */
//	BlendFactor destinationBlend;
//
//	/** カリング方法 */
//	CullingMode cullingMode;
//
//	/** 塗りつぶし方法 */
//	FillMode fillMode;
//
//	RenderStateDesc();
//};










/** レンダー ターゲットのブレンディングステート */
struct RenderTargetBlendDesc
{
	/** ブレンディングの有無 (default:false) */
	bool blendEnable;

	/** 入力元の RGB に対する係数 (default: One) */
	BlendFactor sourceBlend;

	/** 出力先の RGB に対する係数 (default: Zero) */
	BlendFactor destinationBlend;

	/** RGB のブレンディング操作 (default: Add) */
	BlendOp blendOp;

	/** 入力元のアルファ値に対する係数 (default: One) */
	BlendFactor sourceBlendAlpha;

	/** 出力先のアルファ値に対する係数 (default: Zero) */
	BlendFactor destinationBlendAlpha;

	/** アルファ値のブレンディング操作 (default: Add) */
	BlendOp blendOpAlpha;

	RenderTargetBlendDesc();
};

/** ブレンディングステート */
struct BlendStateDesc
{
	/** レンダーターゲットで独立したブレンディングを有効にするには、true に設定します。(default:false) */
	bool independentBlendEnable;

	/** レンダーターゲットごとのブレンドステートの配列です。 */
	RenderTargetBlendDesc renderTargets[8];

	BlendStateDesc();
};

/** ラスタライザステート */
struct RasterizerStateDesc
{
	/** 塗りつぶし方法 (default:Solid) */
	FillMode fillMode;

	/** カリング方法 (default:Back) */
	CullingMode cullMode;

	RasterizerStateDesc();
};

/** ステンシル処理 */
struct StencilOpDesc
{
	/** ステンシルテストに失敗した場合のステンシル処理です。(default:Keep) */
	StencilOp stencilFailOp;

	/** ステンシルテストに合格で、深度テストが不合格の場合のステンシル処理です。(default:Keep) */
	StencilOp stencilDepthFailOp;

	/** ステンシルテストと深度テストに合格した場合のステンシル処理です。(default:Keep) */
	StencilOp stencilPassOp;

	/** ステンシルテストの比較関数　(default:Always) */
	ComparisonFunc stencilFunc;

	StencilOpDesc();
};

/** 深度ステンシルステート */
struct DepthStencilStateDesc
{
	/** 深度テストの有効状態 (default:true) */
	bool depthTestEnabled;

	/** 深度書き込みの有効状態 (default:true) */
	bool depthWriteEnabled;

	/** ステンシルテストの有効状態 (default:false) */
	bool stencilEnabled;

	/** ステンシルテストの参照値 (default:0) */
	uint8_t stencilReferenceValue;

	/** 法線がカメラの方向を向いている面のステンシル処理 */
	StencilOpDesc frontFace;

	/** 法線がカメラと逆方向を向いている面のステンシル処理 */
	StencilOpDesc backFace;

	DepthStencilStateDesc();
};





#if 0

/** 合成方法 */
enum class BlendMode
{
	Normal,			/**< 通常 */
	Alpha,			/**< アルファブレンド */
	Add,			/**< 加算合成 */
	Subtract,		/**< 減算合成 */
	Multiply,		/**< 乗算合成 */
};

/**
	@brief	グラフィックデバイスのレンダリングステートを表すクラスです。
*/
class RenderState
{
public:
	static const RenderState Default;

	bool			alphaBlendEnabled;	/**< アルファブレンドの有無 */
	BlendOp			blendOp;			/**< ブレンディングの演算方法 (default: Add) */
	BlendFactor		sourceBlend;		/**< ブレンディングの係数 (default: One) */
	BlendFactor		destinationBlend;	/**< ブレンディングの係数 (default: Zero) */
	
	BlendMode		Blend;	/*TODO: 廃止予定*/		///< 合成方法 (default:BlendMode::Alpha)
	CullingMode		Culling;		///< カリング方法 (default:CullingMode_Back)
	FillMode		Fill;			///< 塗りつぶし方法 (default:FillMode_Solid)
	bool			AlphaTest;      ///< アルファテストの有無 (default:true)
	//bool			PointSprite;	///< ポイントスプライトモードの有無 (default:false)

public:

	/**
		@brief	デフォルト値で初期化します。
	*/
	RenderState();

	/**
		@brief	現在の設定のハッシュ値を計算します。
	*/
	uint32_t getHashCode() const;

	bool equals(const RenderState& state) const;

public:
	bool operator == (const RenderState& obj) const;
	bool operator != (const RenderState& obj) const { return !operator==(obj); }
};

/**
	@brief	グラフィックデバイスの深度テスト及びステンシルテストのステートを表すクラスです。
*/
class DepthStencilState
{
public:
	static const DepthStencilState Default;

	bool		DepthTestEnabled;		///< 深度テストの有無 (default:true)
	bool		DepthWriteEnabled;		///< 深度書き込みの有無 (default:true)
	CompareFunc	DepthTestFunc;			///< 深度テストの比較関数 (default:LessEqual)

	bool		StencilEnabled;			///< ステンシルテストの有無 (default:false)
	CompareFunc	StencilFunc;			///< ステンシルテストの比較関数 (default:Always)
	uint8_t		StencilReferenceValue;	///< ステンシルテストの参照値 (default:0)
	StencilOp	StencilFailOp;			///< ステンシルテストに失敗した場合のステンシル処理です。(default:Keep)
	StencilOp	StencilDepthFailOp;		///< ステンシルテストに合格で、深度テストが不合格の場合のステンシル処理です。(default:Keep)
	StencilOp	StencilPassOp;			///< ステンシルテストと深度テストに合格した場合のステンシル処理です。(default:Keep)

public:
	/**
		@brief	デフォルト値で初期化します。
	*/
	DepthStencilState();

	// TODO:
	// OpenGL : デプスバッファでの比較→ステンシルバッファでの比較
	// DirectX : 「ステンシルテスト」→「深度テスト」
	// ・・・らしい。要確認。

	bool equals(const DepthStencilState& state) const;


	bool operator == (const DepthStencilState& state) const { return equals(state); }
	bool operator != (const DepthStencilState& state) const { return !equals(state); }
};
#endif

} // namespace ln
