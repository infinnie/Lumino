﻿
#pragma once
#include "Common.hpp"

namespace ln {

/** 時間の経過をとおして値を評価します。アニメーションが生成する値の最小単位です。 */
class AnimationCurve
	: public Object
{
public:

	/** 指定した時間における値を評価します。*/
	float evaluate(float time);

	/** アニメーションの終端の時間を取得します。 */
	float lastFrameTime() const;

	/** アニメーションの繰り返しの動作を取得します。 */
	AnimationWrapMode wrapMode() const { return m_wrapMode; }

	/**　アニメーションの繰り返しの動作を設定します。(default: Once) */
	void setWrapMode(AnimationWrapMode mode) { m_wrapMode = mode; }

protected:
	virtual float onEvaluate(float time) = 0;
	virtual float onGetLastFrameTime() const = 0;

LN_CONSTRUCT_ACCESS:
	AnimationCurve();
	virtual ~AnimationCurve();

private:
	AnimationWrapMode m_wrapMode;
};

/** キーフレームアニメーションの補間方法 */
enum class TangentMode
{
	/** 線形補間 */
	Linear,

	/** 接線 (速度) を使用した補間 (エルミートスプライン) */
	Tangent,

	/** キーフレームの値を通過するなめらかな補間 (Catmull-Rom) */
	Auto,

	/** 補間なし */
	Constant,
};

/** FloatAnimationCurve のキーフレーム */
struct AnimationKeyFrame
{
	/** 時間 */
	float time;

	/** 値 */
	float value;

	/** 前のキーフレームとの補間方法 */
	TangentMode leftTangentMode;

	/** 前のキーフレームからこのキーフレームに近づくときの接線 */
	float leftTangent;

	/** 次のキーフレームとの補間方法 */
	TangentMode rightTangentMode;

	/** このキーフレームから次のキーフレームに向かうときの接線 */
	float rightTangent;
};

/** キーフレームを用いて補間を行う AnimationCurve です。 */
class KeyFrameAnimationCurve
	: public AnimationCurve
{
public:

	/** 空の KeyFrameAnimationCurve を作成します。 */
	static Ref<KeyFrameAnimationCurve> create();

public:

	/** キーフレームを追加します。 */
	void addKeyFrame(const AnimationKeyFrame& keyFrame);

	/** 
     * キーフレームを追加します。
     *
     * rightTangentMode は、新しく追加するキーフレームの右側の補間方法です。
     * 新しく追加するキーフレームの左側の保管方法は、そのひとつ前のキーフレームの右側の保管方法が設定されます。
     */
	void addKeyFrame(float time, float value, TangentMode rightTangentMode = TangentMode::Linear, float tangent = 0.0f);

LN_CONSTRUCT_ACCESS:
	KeyFrameAnimationCurve();
	virtual ~KeyFrameAnimationCurve();

protected:
	// override AnimationCurve
	virtual float onEvaluate(float time) override;
	virtual float onGetLastFrameTime() const override;

private:
	AnimationKeyFrame * findKeyFrame(float time);

	List<AnimationKeyFrame>	m_keyFrames;
	float m_defaultValue;
};

} // namespace ln