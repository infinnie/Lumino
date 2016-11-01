﻿
#include "../Internal.h"
#include <Lumino/Graphics/Color.h>
#include <Lumino/Graphics/Rendering.h>
#include <Lumino/Graphics/ContextInterface.h>
#include <Lumino/Graphics/Mesh.h>
#include "Device/GraphicsDriverInterface.h"
#include "GraphicsManager.h"
#include "RendererImpl.h"
#include "PrimitiveRenderer.h"
#include "MeshRendererProxy.h"
#include "SpriteRenderer.h"

LN_NAMESPACE_BEGIN

namespace detail {

//==============================================================================
// CommandDataCache
//==============================================================================

//------------------------------------------------------------------------------
CommandDataCache::CommandDataCache()
	: m_dataList()
	, m_dataBuffer()
	, m_dataBufferUsed(0)
{
}

//------------------------------------------------------------------------------
CommandDataCache::~CommandDataCache()
{
}

//------------------------------------------------------------------------------
void CommandDataCache::Reserve(size_t dataCount, size_t byteCount)
{
	Clear();
	m_dataList.Reserve(dataCount);
	m_dataBuffer.Resize(byteCount, false);
}

//------------------------------------------------------------------------------
void CommandDataCache::Clear()
{
	m_dataList.Clear();
	m_dataBufferUsed = 0;
}

//------------------------------------------------------------------------------
CommandDataCache::DataHandle CommandDataCache::AllocData(size_t byteCount)
{
	// バッファが足りなければ拡張する
	if (m_dataBufferUsed + byteCount > m_dataBuffer.GetSize())
	{
		size_t newSize = m_dataBuffer.GetSize() + std::max(m_dataBuffer.GetSize(), byteCount);	// 最低でも byteCount 分を拡張する
		m_dataBuffer.Resize(newSize, false);
	}

	//if (copyData != nullptr)
	//{
	//	byte_t* ptr = &(m_commandDataBuffer.GetData()[m_commandDataBufferUsed]);
	//	size_t size = m_commandDataBuffer.GetSize() - m_commandDataBufferUsed;
	//	memcpy_s(ptr, size, copyData, byteCount);
	//}

	size_t dataIdx = m_dataBufferUsed;
	m_dataList.Add(dataIdx);
	m_dataBufferUsed += byteCount;
	return dataIdx;
}

//------------------------------------------------------------------------------
byte_t* CommandDataCache::GetData(DataHandle handle)
{
	return &(m_dataBuffer.GetData()[handle]);
}


//==============================================================================
// InternalContext
//==============================================================================

//------------------------------------------------------------------------------
InternalContext::InternalContext()
	: m_baseRenderer(nullptr)
	, m_primitiveRenderer(nullptr)
	, m_meshRenderer(nullptr)
	, m_spriteRenderer(nullptr)
	, m_current(nullptr)
{
}

//------------------------------------------------------------------------------
void InternalContext::Initialize(detail::GraphicsManager* manager)
{
	m_baseRenderer = manager->GetRenderer();

	m_primitiveRenderer = RefPtr<PrimitiveRenderer>::MakeRef();
	m_primitiveRenderer->Initialize(manager);

	m_meshRenderer = RefPtr<MeshRendererProxy>::MakeRef();
	m_meshRenderer->Initialize(manager);

	m_spriteRenderer = RefPtr<SpriteRenderer>::MakeRef(manager, 2048);	// TODO
}

//------------------------------------------------------------------------------
Details::Renderer* InternalContext::GetRenderStateManager()
{
	return m_baseRenderer;
}

//------------------------------------------------------------------------------
Details::Renderer* InternalContext::BeginBaseRenderer()
{
	SwitchActiveRenderer(m_baseRenderer);
	return m_baseRenderer;
}

//------------------------------------------------------------------------------
PrimitiveRenderer* InternalContext::BeginPrimitiveRenderer()
{
	SwitchActiveRenderer(m_primitiveRenderer);
	return m_primitiveRenderer;
}

//------------------------------------------------------------------------------
MeshRendererProxy* InternalContext::BeginMeshRenderer()
{
	SwitchActiveRenderer(m_meshRenderer);
	return m_meshRenderer;
}

//------------------------------------------------------------------------------
SpriteRenderer* InternalContext::BeginSpriteRenderer()
{
	SwitchActiveRenderer(m_spriteRenderer);
	return m_spriteRenderer;
}

//------------------------------------------------------------------------------
void InternalContext::SetViewInfo(const SizeF& viewPixelSize, const Matrix& viewMatrix, const Matrix& projMatrix)
{
	m_spriteRenderer->SetViewInfo(viewPixelSize, viewMatrix, projMatrix);
	m_primitiveRenderer->SetViewPixelSize(SizeI(viewPixelSize.width, viewPixelSize.height));
	m_primitiveRenderer->SetViewProjMatrix(viewMatrix * projMatrix);
}

//------------------------------------------------------------------------------
SpriteRenderer* InternalContext::GetSpriteRenderer()
{
	return m_spriteRenderer;
}

//------------------------------------------------------------------------------
void InternalContext::Flush()
{
	if (m_current != nullptr) m_current->Flush();
}

//------------------------------------------------------------------------------
void InternalContext::SwitchActiveRenderer(detail::IRendererPloxy* renderer)
{
	if (m_current != renderer)
	{
		if (m_current != nullptr) m_current->OnDeactivated();
		m_current = renderer;
		if (m_current != nullptr) m_current->OnActivated();
	}
}

//==============================================================================
// DrawElementBatch
//==============================================================================

//------------------------------------------------------------------------------
DrawElementBatch::DrawElementBatch()
{
	Reset();
}

//------------------------------------------------------------------------------
void DrawElementBatch::SetMaterial(Material* value)
{
	if (m_material != value)
	{
		m_material = value;
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
bool DrawElementBatch::Equal(const DrawElementBatch& obj) const
{
#if 1
	return GetHashCode() == obj.GetHashCode();
#else
	if (m_rendererId != obj.m_rendererId) return false;

	if (m_alphaBlendEnabled != obj.m_alphaBlendEnabled) return false;
	if (m_blendMode != obj.m_blendMode) return false;
	if (m_cullingMode != obj.m_cullingMode) return false;
	if (m_alphaTestEnabled != obj.m_alphaTestEnabled) return false;

	if (m_depthTestEnabled != obj.m_depthTestEnabled) return false;
	if (m_depthWriteEnabled != obj.m_depthWriteEnabled) return false;

	if (m_shaderPass != obj.m_shaderPass) return false;
	if (m_shaderValueList.GetCount() != obj.m_shaderValueList.GetCount()) return false;
	for (int i = 0; i < m_shaderValueList.GetCount(); ++i)
	{
		if (m_shaderValueList[i].variable != obj.m_shaderValueList[i].variable) return false;
		if (!m_shaderValueList[i].value.Equals(obj.m_shaderValueList[i].value)) return false;
	}

	for (int i = 0; i < MaxMultiRenderTargets; ++i)
	{
		if (m_renderTargets[i] != obj.m_renderTargets[i]) return false;
	}
	if (m_depthBuffer != obj.m_depthBuffer) return false;
	if (m_scissorRect != obj.m_scissorRect) return false;

	return true;
#endif
}

//------------------------------------------------------------------------------
void DrawElementBatch::Reset()
{
	m_alphaBlendEnabled = RenderState::Default.alphaBlendEnabled;
	m_cullingMode = RenderState::Default.Culling;
	m_alphaTestEnabled = RenderState::Default.AlphaTest;
	m_blendMode = BlendMode::Normal;

	m_depthTestEnabled = DepthStencilState::Default.DepthTestEnabled;
	m_depthWriteEnabled = DepthStencilState::Default.DepthWriteEnabled;

	for (int i = 0; i < MaxMultiRenderTargets; ++i)
		m_renderTargets[i] = nullptr;
	m_depthBuffer = nullptr;

	m_material = nullptr;
	//m_shaderValueList.Clear();
	//m_shaderPass = nullptr;

	m_hashCode = 0;
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void DrawElementBatch::ApplyStatus(InternalContext* context, RenderTarget* defaultRenderTarget, DepthBuffer* defaultDepthBuffer)
{
	auto* stateManager = context->GetRenderStateManager();

	// RenderState
	{
		RenderState state;
		state.alphaBlendEnabled = m_alphaBlendEnabled;
		state.Culling = m_cullingMode;
		state.AlphaTest = m_alphaTestEnabled;
		ContextInterface::MakeBlendMode(m_blendMode, &state);
		stateManager->SetRenderState(state);

		// スプライトバッチ化のため
		context->GetSpriteRenderer()->SetState(state);
	}
	// DepthStencilState
	{
		DepthStencilState state;
		state.DepthTestEnabled = m_depthTestEnabled;
		state.DepthWriteEnabled = m_depthWriteEnabled;
		stateManager->SetDepthStencilState(state);
	}
	// FrameBuffer
	{
		for (int i = 0; i < MaxMultiRenderTargets; ++i)
		{
			if (i == 0 && m_renderTargets[i] == nullptr)
				stateManager->SetRenderTarget(i, defaultRenderTarget);
			else
				stateManager->SetRenderTarget(i, m_renderTargets[i]);
		}
		if (m_depthBuffer == nullptr)
			stateManager->SetDepthBuffer(defaultDepthBuffer);
		else
			stateManager->SetDepthBuffer(m_depthBuffer);
		// TODO: m_scissorRect
	}
	// Shader
	if (m_material != nullptr && m_material->GetShader() != nullptr)
	{
		m_material->ApplyToShaderVariables();
		Shader* shader = m_material->GetShader();
		ShaderPass* pass = shader->GetTechniques().GetAt(0)->GetPasses().GetAt(0);	// TODO: DrawList の実行者によって決定する
		stateManager->SetShaderPass(pass);
	}
	//for (ShaderValuePair& pair : m_shaderValueList)
	//{
	//	pair.variable->SetShaderValue(pair.value);
	//}
	//stateManager->SetShaderPass(m_shaderPass);
}

//------------------------------------------------------------------------------
size_t DrawElementBatch::GetHashCode() const
{
	if (m_hashDirty)
	{
		m_hashCode = 0;
		m_hashDirty = false;
		m_hashCode = Hash::CalcHash(reinterpret_cast<const char*>(this), sizeof(DrawElementBatch));
	}
	return m_hashCode;
}

//==============================================================================
// DrawElement
//==============================================================================

//------------------------------------------------------------------------------
DrawElement::DrawElement()
	: transform()
	, batchIndex(-1)
	, drawingSectionId(DrawingSectionId::None)
{
	boundingSphere.center = Vector3::Zero;
	boundingSphere.radius = -1.0f;
}

//------------------------------------------------------------------------------
DrawElement::~DrawElement()
{
}

//------------------------------------------------------------------------------
void DrawElement::MakeBoundingSphere(const Vector3& minPos, const Vector3& maxPos)
{
	Vector3 center = minPos + ((maxPos - minPos) / 2);
	boundingSphere.center = center;
	boundingSphere.radius = std::max(Vector3::Distance(minPos, center), Vector3::Distance(maxPos, center));
}

//==============================================================================
// DrawElementList
//==============================================================================

//------------------------------------------------------------------------------
DrawElementList::DrawElementList()
{
	m_commandDataCache.Reserve(512, 4096);	// 適当に
	m_extDataCache.Reserve(512, 4096);
}

//------------------------------------------------------------------------------
void DrawElementList::ClearCommands()
{
	for (int i = 0; i < GetElementCount(); ++i)
	{
		DrawElement* cmd = GetElement(i);
		cmd->~DrawElement();
	}

	m_commandDataCache.Clear();
	m_extDataCache.Clear();
}

//------------------------------------------------------------------------------
void DrawElementList::PostAddCommandInternal(const DrawElementBatch& state, detail::IRendererPloxy* renderer, DrawElement* element)
{
	if (m_batchList.IsEmpty() || !m_batchList.GetLast().Equal(state))
	{
		element->batchIndex = m_batchList.GetCount();
		m_batchList.Add(state);
		m_batchList.GetLast().m_rendererId = reinterpret_cast<intptr_t>(renderer);
	}
}


//==============================================================================
// InternalRenderer
//==============================================================================

//------------------------------------------------------------------------------
InternalRenderer::InternalRenderer()
{
}

//------------------------------------------------------------------------------
InternalRenderer::~InternalRenderer()
{
}

//------------------------------------------------------------------------------
void InternalRenderer::Initialize(GraphicsManager* manager)
{
	m_manager = manager;
}

//------------------------------------------------------------------------------
void InternalRenderer::Render(
	DrawElementList* elementList,
	const SizeF& viewPixelSize,
	const Matrix& viewMatrix,
	const Matrix& projMatrix,
	const ViewFrustum& viewFrustum,
	RenderTarget* defaultRenderTarget,
	DepthBuffer* defaultDepthBuffer)
{
	InternalContext* context = m_manager->GetInternalContext();
	m_renderingElementList.Clear();

	// 視点に関する情報の設定
	context->SetViewInfo(viewPixelSize, viewMatrix, projMatrix);

	// 視錘台カリング
	for (int i = 0; i < elementList->GetElementCount(); ++i)
	{
		DrawElement* element = elementList->GetElement(i);
		Sphere boundingSphere = element->GetBoundingSphere();

		if (boundingSphere.radius < 0 ||	// マイナス値なら視錐台と衝突判定しない
			viewFrustum.Intersects(boundingSphere.center, boundingSphere.radius))
		{
			// このノードは描画できる
			m_renderingElementList.Add(element);
		}
	}

	// 描画
	int currentBatchIndex = -1;
	for (DrawElement* element : m_renderingElementList)
	{
		// ステートの変わり目チェック
		if (element->batchIndex != currentBatchIndex)
		{
			currentBatchIndex = element->batchIndex;
			elementList->GetBatch(currentBatchIndex)->ApplyStatus(context, defaultRenderTarget, defaultDepthBuffer);
		}

		// 描画実行
		element->Execute(context);
	}
}

//==============================================================================
// ScopedStateBlock2
//==============================================================================

//------------------------------------------------------------------------------
ScopedStateBlock2::ScopedStateBlock2(DrawList* renderer)
	: m_renderer(renderer)
	, m_state(renderer->GetState())
{}

//------------------------------------------------------------------------------
ScopedStateBlock2::~ScopedStateBlock2()
{
	Apply();
}

//------------------------------------------------------------------------------
void ScopedStateBlock2::Apply()
{
	m_renderer->SetState(m_state);
}

//==============================================================================
class ClearElement : public DrawElement
{
public:
	ClearFlags flags;
	Color color;
	float z;
	uint8_t stencil;

	virtual void Execute(InternalContext* context)
	{
		context->BeginBaseRenderer()->Clear(flags, color, z, stencil);
	}
};

//==============================================================================
class DrawSpriteElement : public DrawElement
{
public:
	Vector3 position;
	Vector2 size;
	Vector2 anchorRatio;
	RefPtr<Texture> texture;
	RectF srcRect;
	Color color;

	virtual void Execute(InternalContext* context)
	{
		context->BeginSpriteRenderer()->DrawRequest2D(position, size, anchorRatio, texture, srcRect, color);
	}
};

} // namespace detail 

//==============================================================================
// DrawList
//==============================================================================



//------------------------------------------------------------------------------
DrawList::DrawList()
	: m_currentSectionTopElement(nullptr)
{
}

//------------------------------------------------------------------------------
DrawList::~DrawList()
{
}

//------------------------------------------------------------------------------
void DrawList::Initialize(detail::GraphicsManager* manager)
{
	LN_CHECK_ARG(manager != nullptr);
	m_manager = manager;
	Clear();
}

//------------------------------------------------------------------------------
void DrawList::Clear()
{
	m_drawElementList.ClearCommands();
	m_state.Reset();
}

//------------------------------------------------------------------------------
void DrawList::EndFrame()
{
	m_manager->GetInternalContext()->Flush();
}

////------------------------------------------------------------------------------
//void DrawList::BeginFrame(RenderTarget* defaultRenderTarget, DepthBuffer* defaultDepthBuffer)
//{
//	m_state.state.m_renderTargets[0] = defaultRenderTarget;
//	m_state.state.m_depthBuffer = defaultDepthBuffer;
//}

//------------------------------------------------------------------------------
void DrawList::SetTransform(const Matrix& transform)
{
	m_state.transfrom = transform;
}

//------------------------------------------------------------------------------
void DrawList::Clear(ClearFlags flags, const Color& color, float z, uint8_t stencil)
{
	auto* ptr = m_drawElementList.AddCommand<detail::ClearElement>(m_state.state, m_manager->GetInternalContext()->m_baseRenderer);
	ptr->flags = flags;
	ptr->color = color;
	ptr->z = z;
	ptr->stencil = stencil;
}

//------------------------------------------------------------------------------
void DrawList::DrawLinePrimitive(
	const Vector3& position1, const Color& color1,
	const Vector3& position2, const Color& color2)
{
	// TODO: キャッシュと LineStrip
	class DrawElement_DrawLine : public detail::DrawElement
	{
	public:
		Vector3 position1; Color color1;
		Vector3 position2; Color color2;

		virtual void Execute(detail::InternalContext* context) override
		{
			context->BeginPrimitiveRenderer()->DrawLine(
				position1, color1, position2, color2);
		}
	};
	auto* ptr = ResolveDrawElement<DrawElement_DrawLine>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer);
	ptr->position1 = position1; ptr->color1 = color1;
	ptr->position2 = position2; ptr->color2 = color2;
	ptr->MakeBoundingSphere(Vector3::Min(position1, position2), Vector3::Max(position1, position2));
}

//------------------------------------------------------------------------------
void DrawList::DrawSquarePrimitive(
	const Vector3& position1, const Vector2& uv1, const Color& color1,
	const Vector3& position2, const Vector2& uv2, const Color& color2,
	const Vector3& position3, const Vector2& uv3, const Color& color3,
	const Vector3& position4, const Vector2& uv4, const Color& color4/*,
	ShaderPass* shaderPass*/)
{
	class DrawSquarePrimitiveElement : public detail::DrawElement
	{
	public:
		Vector3 position[4];
		Vector2 uv[4];
		Color color[4];

		virtual void Execute(detail::InternalContext* context) override
		{
			context->BeginPrimitiveRenderer()->DrawSquare(
				position[0], uv[0], color[0],
				position[1], uv[1], color[1],
				position[2], uv[2], color[2],
				position[3], uv[3], color[3]);
		}
	};
	auto* ptr = ResolveDrawElement<DrawSquarePrimitiveElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_primitiveRenderer);
	ptr->position[0] = position1; ptr->uv[0] = uv1; ptr->color[0] = color1;
	ptr->position[1] = position2; ptr->uv[1] = uv2; ptr->color[1] = color2;
	ptr->position[2] = position3; ptr->uv[2] = uv3; ptr->color[2] = color3;
	ptr->position[3] = position4; ptr->uv[3] = uv4; ptr->color[3] = color4;
	ptr->MakeBoundingSphere(Vector3::Min(ptr->position, 4), Vector3::Max(ptr->position, 4));
}

//------------------------------------------------------------------------------
void DrawList::DrawSprite2D(
	const SizeF& size,
	Texture* texture,
	const RectF& srcRect,
	const Color& color)
{
	auto* ptr = ResolveDrawElement<detail::DrawSpriteElement>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_spriteRenderer);
	//ptr->position;
	ptr->size.Set(size.width, size.height);
	//ptr->anchorRatio;
	ptr->texture = texture;
	ptr->srcRect = srcRect;
	ptr->color = color;
}

//------------------------------------------------------------------------------
void DrawList::DrawMesh(StaticMeshModel* mesh, int submeshIndex, Material* material)
{
	const MeshAttribute& attr = mesh->GetMeshResource()->m_attributes[submeshIndex];
	DrawMeshInternal(mesh, attr.StartIndex, attr.PrimitiveNum, material);
}

//------------------------------------------------------------------------------
template<typename TElement>
TElement* DrawList::ResolveDrawElement(detail::DrawingSectionId sectionId, detail::IRendererPloxy* renderer)
{
	// 何か前回追加された DrawElement があり、それと DrawingSectionId、State が一致するならそれに対して追記できる
	if (sectionId != detail::DrawingSectionId::None &&
		m_currentSectionTopElement != nullptr &&
		m_currentSectionTopElement->drawingSectionId == sectionId &&
		m_drawElementList.GetBatch(m_currentSectionTopElement->batchIndex)->Equal(m_state.state))
	{
		return static_cast<TElement*>(m_currentSectionTopElement);
	}

	// DrawElement を新しく作る
	TElement* element = m_drawElementList.AddCommand<TElement>(m_state.state, renderer);
	m_currentSectionTopElement = element;
	return element;
}

//detail::DrawElement* DrawList::StartDrawSection(detail::DrawingSectionId sectionId)
//{
//	if (m_currentSectionTopElement)
//	{
//
//	}
//	return nullptr;
//}

//------------------------------------------------------------------------------
void DrawList::DrawMeshInternal(StaticMeshModel* mesh, int startIndex, int triangleCount, Material* material)
{
	/* 
	 * この時点では MeshResource ではなく StaticMeshModel が必要。
	 * LOD リソースがある場合、実際に書くときの視点情報を元に、描画する LOD リソースを選択する必要がある。
	 */

	class DrawElement_DrawMeshInternal : public detail::DrawElement
	{
	public:
		RefPtr<StaticMeshModel>	mesh;
		int startIndex;
		int triangleCount;

		virtual void Execute(detail::InternalContext* context) override
		{
			context->BeginMeshRenderer()->DrawMesh(mesh->GetMeshResource(), startIndex, triangleCount);
		}
	};

	m_state.state.SetMaterial(material);

	auto* e = ResolveDrawElement<DrawElement_DrawMeshInternal>(detail::DrawingSectionId::None, m_manager->GetInternalContext()->m_meshRenderer);
	e->mesh = mesh;
	e->startIndex = startIndex;
	e->triangleCount = triangleCount;
	//e->boundingSphere = ;	// TODO
}

//------------------------------------------------------------------------------
void DrawList::BltInternal(Texture* source, RenderTarget* dest, const Matrix& transform, Shader* shader)
{

}

LN_NAMESPACE_END

