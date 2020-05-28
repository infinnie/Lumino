﻿#pragma once
#include "RenderStage.hpp"
#include "BlitRenderFeature.hpp"
#include "SpriteRenderFeature.hpp"
#include "MeshRenderFeature.hpp"
#include "PrimitiveRenderFeature.hpp"
#include "SpriteTextRenderFeature.hpp"
#include "FrameRectRenderFeature.hpp"
#include "ShapesRenderFeature.hpp"
#include "ExtensionRenderFeature.hpp"

namespace ln {
class RenderViewPoint;


namespace detail {

enum class StateStackMode
{
	Default,
	ScissorPushPop,
};

// CommandBuffer から使われることを想定。
// draw 系を呼んだときに、ステートが変化していれば新しい RenderStage を作ったり、
// 0.4.0 まで DrawList クラスがやってたことを担当する。
// CommandBuffer や DrawElementList と分離しているのは、
// - 一部のステートセットをユーザーから隠蔽したい
//		- → VisualNode のステートセット
// - メモリ効率をよくしたい
//		- → CommandBuffer や DrawElementList はたくさん作られることがあるが、ステート変化検出用の一時ステートを全部持ち続けるのは無駄。
//			DrawElementListBuilder は EngineDomain 内の唯一インスタンスでもよい。（並列化はできなくなるが・・・まぁ、その場合はインスタンスを複数作ればいいだけ）
class DrawElementListBuilder
	: public RefObject
{
public:
	StateStackMode m_stateStackMode = StateStackMode::Default;	// TODO: private

	DrawElementListBuilder();
    virtual ~DrawElementListBuilder();
	DrawElementList* targetList() const { return m_targetList; }
	void setTargetList(DrawElementList* targetList);
    void resetForBeginRendering();  // 描画開始時のリセット。スタックもクリアする
	void reset2();   // スタックに積んである分はリセットしないしスタックも消さない
    void advanceFence();	// zソートなどの境界。特に、clear や blit など、深度が関係ないものとの境界

	void setRenderTarget(int index, RenderTargetTexture* value);
    RenderTargetTexture* renderTarget(int index) const;
	void setDepthBuffer(DepthBuffer* value);
	void setViewportRect(const RectI& value);
	void setScissorRect(const RectI& value);

	void setBlendMode(const Optional<BlendMode>& value);
	void setCullingMode(const Optional<CullMode>& value);
	void setDepthTestEnabled(const Optional<bool>& value);
	void setDepthWriteEnabled(const Optional<bool>& value);
	void setPrimitiveTopology(PrimitiveTopology value);

	void setShadingModel(const Optional<ShadingModel>& value);
	void setMaterial(Material* value);  // 一度 set したマテリアルは描画完了まで変更してはならない。TODO: Freezed みたいな状態にしたい
    Material* material() const;
	void setTransfrom(const Matrix& value);
    void setBaseTransfrom(const Optional<Matrix>& value);
	const Matrix& baseTransform() const;
    void setRenderPriority(int value);
    void setRenderPhase(RenderPhaseClass value);
	void setAdditionalElementFlags(RenderDrawElementTypeFlags value);

	// BuiltinEffectData
	void setOpacity(float value);
	void setColorScale(const Color& value);
	void setBlendColor(const Color& value);
	void setTone(const ColorTone& value);
    void setBaseBuiltinEffectData(const Optional<BuiltinEffectData>& value);

	void setFont(Font* value);
	const Ref<Font>& font() const { return primaryStateConst()->font; }
	void setTextColor(const Color& value);
	const Color& textColor() const { return primaryStateConst()->textColor; }

    void setViewPoint(RenderViewPoint* value);
    RenderViewPoint* viewPoint() const;

    void pushState(bool reset); // ※ 単純に state を退避するための仕組みなので、OpenGL の push/pop Matrix のように transform の乗算などは行わない。
    void popState();

    BlitRenderFeatureStageParameters* blitRenderFeatureStageParameters() { return &m_blitRenderFeatureStageParameters; }
	SpriteRenderFeatureStageParameters* spriteRenderFeatureStageParameters() { return &m_spriteRenderFeatureStageParameters; }
	SpriteRenderFeatureStageParameters2* spriteRenderFeatureStageParameters2() { return &m_spriteRenderFeatureStageParameters2; }
	MeshRenderFeatureStageParameters* meshRenderFeatureStageParameters() { return &m_meshRenderFeatureStageParameters; }
	MeshGeneraterRenderFeatureStageParameters* meshGeneraterRenderFeatureStageParameters() { return &m_meshGeneraterRenderFeatureStageParameters; }
	SpriteTextRenderFeatureStageParameters* spriteTextRenderFeatureStageParameters() { return &m_spriteTextRenderFeatureStageParameters; }
	FrameRectRenderFeatureStageParameters* frameRectRenderFeatureStageParameters() { return &m_frameRectRenderFeatureStageParameters; }
	ShapesRenderFeatureStageParameters* shapesRenderFeatureStageParameters() { return &m_shapesRenderFeatureStageParameters; }
    ExtensionRenderFeatureStageParameters* extensionRenderFeatureStageParameters() { return &m_extensionRenderFeatureStageParameters; }

	template<class TElement>
	TElement* addNewDrawElement(
		RenderFeature* renderFeature,
		RenderFeatureStageParameters* params)
	{
		RenderStage* stage = prepareRenderStage(renderFeature, params);
		if (LN_ENSURE(stage)) return nullptr;
		TElement* element = m_targetList->newFrameData<TElement>();
		m_targetList->addElement(stage, element);
        prepareRenderDrawElement(element, m_targetList->lastElement(), stage);
		return element;
	}

private:
    class State : public RefObject
    {
    public:
        // Infomation for RenderStage
        FrameBufferStageParameters frameBufferStageParameters;
        GeometryStageParameters geometryStageParameters;

        // Infomation for RenderDrawElement
        BuiltinEffectData builtinEffectData;
        Optional<BuiltinEffectData> baseBuiltinEffectData;
        Matrix transform;
        Optional<Matrix> baseTransform;
        int renderPriority;
        RenderPhaseClass rendringPhase;
		RenderDrawElementTypeFlags additionalElementFlags;
		Ref<Font> font;
		Color textColor;

        RenderViewPoint* viewPoint; // DrawElement には流れない、RenderingContext を使う人のための情報

        void reset();
        void copyFrom(const State* other);
    };

    enum class DirtyFlags
    {
        None = 0x00,
        BuiltinEffect = 0x02,
        All = 0xFFFF,
    };

	RenderStage* prepareRenderStage(RenderFeature* renderFeature, RenderFeatureStageParameters* featureParams);
    void prepareRenderDrawElement(RenderDrawElement* newElement, RenderDrawElement* lastElement, RenderStage* stage);
    const Ref<State>& primaryState() { return m_aliveStateStack.front(); }
    const Ref<State>& primaryStateConst() const { return m_aliveStateStack.front(); }
    FrameBufferStageParameters& primaryFrameBufferStageParameters() { return m_aliveStateStack.front()->frameBufferStageParameters; }
    const FrameBufferStageParameters& primaryFrameBufferStageParameters() const { return m_aliveStateStack.front()->frameBufferStageParameters; }
    GeometryStageParameters& primaryGeometryStageParameters() { return m_aliveStateStack.front()->geometryStageParameters; }
    const GeometryStageParameters& primaryGeometryStageParameters() const { return m_aliveStateStack.front()->geometryStageParameters; }

	DrawElementList* m_targetList;
	//FrameBufferStageParameters m_primaryFrameBufferStageParameters;
	//GeometryStageParameters m_primaryGeometryStageParameters;
    List<Ref<State>> m_freeStateStack;
    List<Ref<State>> m_aliveStateStack;	// size >= 1
    int m_currentCommandFence;


	// 以下、各 RenderFeature のステート。
	// これは m_modified 対象外。代わりに prepareRenderStage() のたびに equals() でチェックされる。
	// ユーザー定義する場合は外部でこれらを定義し、draw 時にそのポインタを指定する必要がある。
	// (もしフレームワークに沿った流れでなくても大丈夫ならグローバル変数とかで受け取ったりしても大丈夫)
    BlitRenderFeatureStageParameters m_blitRenderFeatureStageParameters;
	SpriteRenderFeatureStageParameters m_spriteRenderFeatureStageParameters;
	SpriteRenderFeatureStageParameters2 m_spriteRenderFeatureStageParameters2;
	MeshRenderFeatureStageParameters m_meshRenderFeatureStageParameters;
	MeshGeneraterRenderFeatureStageParameters m_meshGeneraterRenderFeatureStageParameters;
	SpriteTextRenderFeatureStageParameters m_spriteTextRenderFeatureStageParameters;
	FrameRectRenderFeatureStageParameters m_frameRectRenderFeatureStageParameters;
	ShapesRenderFeatureStageParameters m_shapesRenderFeatureStageParameters;
    ExtensionRenderFeatureStageParameters m_extensionRenderFeatureStageParameters;

    Ref<Material> m_defaultMaterial;
    Flags<DirtyFlags> m_dirtyFlags;
	bool m_modified;

};

} // namespace detail
} // namespace ln

