﻿
#include "Internal.h"
#include <Lumino/UI/UIViewport.h>
#include "UIManager.h"

LN_NAMESPACE_BEGIN

//==============================================================================
// UIViewport
//==============================================================================
LN_UI_TYPEINFO_IMPLEMENT(UIViewport, UIElement);

//------------------------------------------------------------------------------
UIViewport::UIViewport()
	: UIElement()
	, m_backbufferSize(0, 0)
	, m_placement(ViewportPlacement::Stretch)
{
}

//------------------------------------------------------------------------------
UIViewport::~UIViewport()
{
}

//------------------------------------------------------------------------------
void UIViewport::Initialize()
{
	UIElement::Initialize();
	m_backgroundColor = Color::Blue;
}

//------------------------------------------------------------------------------
void UIViewport::SetBackbufferSize(int width, int height)
{
	m_backbufferSize.Set(width, height);
}

//------------------------------------------------------------------------------
bool UIViewport::OnEvent(detail::UIInternalEventType type, UIEventArgs* args)
{
	return UIElement::OnEvent(type, args);
}

//------------------------------------------------------------------------------
Size UIViewport::ArrangeOverride(const Size& finalSize)
{
	Size renderSize = UIElement::ArrangeOverride(finalSize);


	return renderSize;
}

//------------------------------------------------------------------------------
void UIViewport::OnRender(DrawingContext* g)
{
	// バックバッファサイズの調整
	{
		SizeI bakcbufferSize;
		if (m_placement == ViewportPlacement::AutoResize)
			bakcbufferSize = SizeI::FromFloatSize(GetRenderSize());
		else
			bakcbufferSize = m_backbufferSize;

		UpdateFramebufferSizeIfNeeded(bakcbufferSize);
	}

	UIElement::OnRender(g);
}

//------------------------------------------------------------------------------
void UIViewport::UpdateFramebufferSizeIfNeeded(const SizeI& viewSize)
{
	bool create = false;
	SizeI newSize(0, 0);

	// 初回、まだ作成されていなければ作りたい
	if (m_primaryLayerTarget == nullptr)
	{
		create = true;
		newSize = viewSize;
	}

	// 自動リサイズONで、描画先とサイズが異なるなら再作成
	if (m_placement == ViewportPlacement::AutoResize)
	{
		if (m_primaryLayerTarget != nullptr && viewSize != m_primaryLayerTarget->GetSize())
		{
			create = true;
		}
		newSize = viewSize;
	}

	if (create)
	{
		// RenderTargetTexture
		// TODO: できればこういうのは Resize 関数を作りたい。作り直したくない
		// TODO: というか UE4 みたいにキャッシュしたい
		m_primaryLayerTarget = RefPtr<RenderTargetTexture>::MakeRef();
		m_primaryLayerTarget->CreateImpl(GetManager()->GetGraphicsManager(), newSize, 1, TextureFormat::R8G8B8X8);
		m_secondaryLayerTarget = RefPtr<RenderTargetTexture>::MakeRef();
		m_secondaryLayerTarget->CreateImpl(GetManager()->GetGraphicsManager(), newSize, 1, TextureFormat::R8G8B8X8);

		// DepthBuffer
		m_depthBuffer = RefPtr<DepthBuffer>::MakeRef();
		m_depthBuffer->CreateImpl(GetManager()->GetGraphicsManager(), newSize, TextureFormat::D24S8);
	}
}

//------------------------------------------------------------------------------
void UIViewport::MakeViewBoxTransform(const SizeI& dstSize, const SizeI& srcSize, Matrix* mat)
{
	float sw = static_cast<float>(srcSize.width);   // 転送元
	float sh = static_cast<float>(srcSize.height);
	float dw = static_cast<float>(dstSize.width);	// 転送先
	float dh = static_cast<float>(dstSize.height);

	float new_x, new_y;
	float new_w, new_h;

	float ratio_w;
	float ratio_h;

	// バックバッファサイズとスクリーンサイズが同じ場合
	if (sw == dw && sh == dh)
	{
		// そのまま設定
		new_x = 0;
		new_y = 0;
		new_w = sw;
		new_h = sh;
		ratio_w = 1.0f;
		ratio_h = 1.0f;
	}
	else
	{
		// 現在のスクリーンサイズ(デフォルトビューポートのサイズ)と画面サイズの比率計算
		ratio_w = dw / sw;
		ratio_h = dh / sh;

		// 縦方向に合わせる ( 左右が余る )
		if (ratio_w > ratio_h)
		{
			new_w = static_cast< float >(sw * ratio_h);
			new_h = static_cast< float >(dh);
			new_x = static_cast< float >((dw / 2) - (new_w / 2));
			new_y = 0;
		}
		//横方向にあわせる
		else
		{
			new_w = static_cast< float >(dw);
			new_h = static_cast< float >(sh * ratio_w);
			new_x = 0;
			new_y = static_cast< float >((dh / 2) - (new_h / 2));
		}
	}

#if 1	// pxel based
	* mat = Matrix::Identity;
	mat->Scale(new_w / sw, new_h / sh, 1.0f);
	mat->Translate(new_x, new_y, 0.0f);
#else	// screen coord based
	*mat = Matrix::Identity;
	mat->Scale(new_w / dw, new_h / dh, 1.0f);
#endif
}
LN_NAMESPACE_END
