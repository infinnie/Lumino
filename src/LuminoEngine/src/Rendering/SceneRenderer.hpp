﻿#pragma once
#include <LuminoEngine/Graphics/RenderState.hpp>
#include <LuminoEngine/Shader/Shader.hpp>
#include <LuminoEngine/Rendering/RenderView.hpp>

namespace ln {
class AbstractMaterial;
namespace detail {
class RenderStage;
class RenderDrawElement;

enum class ZSortDistanceBase
{
	NodeZ,					/**< ノードの Z 値を距離として使用する */
	CameraDistance,			/**< カメラとノードの距離を使用する */
	CameraScreenDistance,	/**< カメラが映すスクリーン平面とノードの距離を使用する */
};


class SceneRendererPass
	: public RefObject
{
public:
	SceneRendererPass();
	virtual ~SceneRendererPass();
	void initialize();

	virtual void onBeginRender(RenderView* renderView);
	virtual void onEndRender(RenderView* renderView);

	// シャドウバッファの作成などのため、パス単位で RenderTarget を切り替えたい場合、ここで frameBuffer にセットする。
	// この時点では GraphicsContext は自由に使ってもよい。ステート復元する必要はない。
	virtual void onBeginPass(RenderView* renderView, GraphicsContext* context, FrameBuffer* frameBuffer);

	// Element の情報と派生 Pass から、最終的に使いたい ShaderTechnique を求める
	virtual ShaderTechnique* selectShaderTechnique(
		ShaderTechniqueClass_MeshProcess requestedMeshProcess,
		Shader* requestedShader,
		ShadingModel requestedShadingModel) = 0;

	RenderingManager* manager() const { return m_manager; }

	ShaderTechniqueClass_ShadingModel tlanslateShadingModel(ShadingModel value) { return (ShaderTechniqueClass_ShadingModel)value; }

private:
	RenderingManager* m_manager;
};

class SceneRenderer
	: public RefObject
{
public:
	void render(
		GraphicsContext* graphicsContext,
		RenderView* renderView,
		const FrameBuffer& defaultFrameBuffer);

protected:
	SceneRenderer();
	void initialize();

	void addPass(SceneRendererPass* pass);

	void renderPass(GraphicsContext* graphicsContext, SceneRendererPass* pass);

	// レンダリング準備として、描画に関係する各種オブジェクト (DrawElement や Light) を収集するフェーズ
	virtual void collect(const detail::CameraInfo& cameraInfo);

	// レンダリング準備として、効率的な描画を行うために収集した各種オブジェクトのソートなどを行う
	void prepare();

	//virtual void onBeginRender() = 0;
	//virtual void onEndRender() = 0;

	virtual void onCollectLight(const DynamicLightInfo& light);
	virtual void onSetAdditionalShaderPassVariables(Shader* shader);

	RenderView* renderingRenderView() const { return m_renderingRenderView; }

private:
	void applyFrameBufferStatus(GraphicsContext* context, RenderStage* stage, const FrameBuffer& defaultFrameBufferInPass);
	void applyGeometryStatus(GraphicsContext* context, RenderStage* stage, AbstractMaterial* priorityMaterial);
	static void makeBlendMode(BlendMode mode, RenderTargetBlendDesc* state);

	detail::RenderingManager* m_manager;
	List<Ref<SceneRendererPass>> m_renderingPassList;

	RenderView* m_renderingRenderView;
	const FrameBuffer* m_defaultFrameBuffer;
	ZSortDistanceBase m_zSortDistanceBase;

	// build by collect().
	List<RenderDrawElement*> m_renderingElementList;

	List<SceneRendererPass*>	m_renderingActualPassList;

	// TODO: ちゃんとした置き場は SkinnedMesh 実装時に考える
	Ref<Texture2D>    m_skinningMatricesTexture;            // GPU スキニング用のテクスチャ
	Ref<Texture2D>    m_skinningLocalQuaternionsTexture;    // GPU スキニング用のテクスチャ
};

} // namespace detail
} // namespace ln

