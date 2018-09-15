﻿#pragma once

namespace ln {

enum class ShadingModel : uint8_t
{
	Default,
	UnLighting,
};

/** 合成方法 */
enum class BlendMode : uint8_t
{
	Normal,			/**< 通常 */
	Alpha,			/**< アルファブレンド */
	Add,			/**< 加算合成 */
	Subtract,		/**< 減算合成 */
	Multiply,		/**< 乗算合成 */
};

/** 3D 空間での基準方向を表す値 */
enum class SpriteBaseDirection
{
	XPlus,			/**< X+ 方向 (右向き) */
	YPlus,			/**< Y+ 方向 (上向き) */
	ZPlus,			/**< Z+ 方向 (奥向き) */
	XMinus,			/**< X- 方向 (左向き) */
	YMinus,			/**< Y- 方向 (下向き) */
	ZMinus,			/**< Z- 方向 (手前向き) */
	Basic2D,		/**< Z+ 方向、左上原点 */
};

/** ビルボードの計算方法 */
enum class BillboardType
{
	None,					/**< ビルボードの計算を行わない */
	ToCameraPoint,			/**< カメラ (ビュー行列) に対して正面を向く */
	ToScreen,				/**< スクリーン (ビュー平面) に対して正面を向く */
	RotY,					/**< Y 軸回転のみ行う */
};

} // namespace ln
