﻿
#pragma once

#include "Color.h"
#include "Texture.h"
#include "Shader.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GRAPHICS_BEGIN

// TODO: MMD 用のデータがいくつか含まれているので、そちらは ModelMaterial に移動したい。
/**
	@brief	マテリアルのクラスです。
*/
class Material
{
public:

	/// 描画オプション
	enum DrawingFlags
	{
		DrawingFlag_CullingDouble = 0x01,	///< 両面描画
		DrawingFlag_GroundShadow = 0x02,	///< 地面影
		DrawingFlag_SelfShadowMap = 0x04,	///< セルフシャドウマップへの描画
		DrawingFlag_SelfShadow = 0x08,		///< セルフシャドウの描画
		DrawingFlag_Edge = 0x10,			///< エッジ描画
	};

	/// スフィアテクスチャの合成モード
	enum SphereMode
	{
		SphereMode_Disable = 0,				///< 無効
		SphereMode_Mul = 1,					///< 乗算(sph)
		SphereMode_Add = 2,					///< 加算(spa)
		SphereMode_SubTexture = 3,			///< サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)
	};

public:
	Color						Diffuse;			///< 物体の色
	Color						Ambient;			///< 環境光
	Color						Specular;			///< 光沢
	Color						Emissive;			///< 物体の発光色 ( 光源の影響を受けない色 )
	float						Power;				///< 光沢の強さ

	RefPtr<Shader>				Shader;				///< シェーダ
	RefPtr<ln::Texture>			Texture;			///< テクスチャ
	RefPtr<ln::Texture>			ToonTexture;		///< [PMD] トゥーンテクスチャ (getToonTexture() setToonTexture()でアクセスする)
	RefPtr<ln::Texture>			SphereTexture;		///< [PMD] スフィアテクスチャ (getSphereTexture() setSphereTexture()でアクセスする)

	Color						ToonColor;			///< [PMD] トゥーンカラー
	Color						EdgeColor;			///< [PMX] エッジカラー
	float						EdgeSize;			///< [PMX] エッジサイズ
	Color						TextureCoe;			///< [PMX] テクスチャ係数
	Color						SphereTextureCoe;	///< [PMX] スフィアテクスチャ係数
	Color						ToonTextureCoe;		///< [PMX] Toonテクスチャ係数
	uint32_t					DrawingFlags;		///< [PMX] 描画オプション (MMDDrawingFlags の組み合わせ)
	SphereMode					SphereMode;			///< [PMX] スフィアテクスチャの合成モード

public:

	/**
		@brief		マテリアルを初期化します。
	*/
	Material()
	{
		Diffuse.Set(1.0f, 1.0f, 1.0f, 1.0f);
		Ambient.Set(0.0f, 0.0f, 0.0f, 0.0f);
		Specular.Set(0.5f, 0.5f, 0.5f, 0.5f);
		Emissive.Set(0.0f, 0.0f, 0.0f, 0.0f);
		Power = 50.0f;
		ToonColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
		EdgeColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		EdgeSize = 0.0f;
		SphereMode = SphereMode_Disable;
		DrawingFlags = 0;
	}
};

LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END
