﻿
#include "Internal.hpp"
#include <LuminoEngine/Graphics/Texture.hpp>
#include <LuminoEngine/Rendering/Material.hpp>
#include <LuminoEngine/Visual/SpriteComponent.hpp>
#include "../Rendering/SpriteRenderFeature.hpp"

namespace ln {

//=============================================================================
// SpriteFrame

SpriteFrame::SpriteFrame()
	: m_sourceRect()
	, m_anchorPoint()
{
}

void SpriteFrame::init()
{
	Object::init();
	m_anchorPoint = Vector2(Math::NaN, Math::NaN);
}

//=============================================================================
// SpriteFrameSet

Ref<SpriteFrameSet> SpriteFrameSet::create(Texture* texture, int frameWidth, int frameHeight, const Vector2& anchorPoint)
{
	return newObject<SpriteFrameSet>(texture, frameWidth, frameHeight, anchorPoint);
}

SpriteFrameSet::SpriteFrameSet()
{
}

void SpriteFrameSet::init()
{
	Object::init();
}

void SpriteFrameSet::init(Texture* texture, int frameWidth, int frameHeight, const Vector2& anchorPoint)
{
	init();

	if (LN_REQUIRE(texture)) return;
	if (LN_REQUIRE(frameWidth > 0)) return;
	if (LN_REQUIRE(frameHeight > 0)) return;
	m_texture = texture;
	m_frames = makeList<Ref<SpriteFrame>>();

	int cols = m_texture->width() / frameWidth;
	int rows = m_texture->height() / frameHeight;

	for (int y = 0; y < rows; y++)
	{
		for (int x = 0; x < cols; x++)
		{
			// TODO: モノによっては大量の小オブジェクトができるので、できればまとめて alloc したりキャッシュしたい
			auto frame = newObject<SpriteFrame>();
			frame->setSourceRect(Rect(x * frameWidth, y * frameHeight, frameWidth, frameHeight));
			frame->setAnchorPoint(anchorPoint);
		}
	}
}

SpriteFrame* SpriteFrameSet::frame(int index) const
{
	if (0 <= index && index < m_frames->size()) {
		return m_frames[index];
	}
	else {
		return nullptr;
	}
}

//=============================================================================
// SpriteComponent
/*
 * 正面方向について
 * --------
 * Luminoは 奥方向を正面としている。なら Sprite も面方向と法線はデフォルトで Z+ を向いた方がよいのでは？
 * → Sprite は普通、常にカメラ方向を向くビルボードとして使われることを想定している。
 *   まぁ、3D 空間に看板オブジェクトみたいに板ポリ置くのに使えないことはないけれど、
 *   それってマップオブジェクトを表現するための Mesh としたほうがいいよね。
 */

SpriteComponent::SpriteComponent()
    : m_material(nullptr)
    , m_size()
	, m_frameIndex(-1)
{
}

SpriteComponent::~SpriteComponent()
{
}

void SpriteComponent::init()
{
    VisualComponent::init();
    m_sourceRect.set(0, 0, -1, -1);
    setSize(Size(1, 1));

    m_material = newObject<Material>();
    //m_material->setEmissive(Color(1,1,1,0.5));
    setBlendMode(BlendMode::Alpha);
}

void SpriteComponent::setTexture(Texture* texture)
{
    m_material->setMainTexture(texture);
}

Texture* SpriteComponent::texture() const
{
    return m_material->mainTexture();
}

void SpriteComponent::setSize(const Size& size)
{
    m_size = size;
}

void SpriteComponent::setSourceRect(const Rect& rect)
{
    m_sourceRect = rect;
}

void SpriteComponent::setFrameSet(SpriteFrameSet* value)
{
	m_frameSet = value;
}

void SpriteComponent::setFrameIndex(int index)
{
	m_frameIndex = index;
}

void SpriteComponent::onRender(RenderingContext* context)
{
	Vector2 abchorPoint(0, 0);
	Rect sourceRect = m_sourceRect;


	if (sourceRect.isEmpty() && m_frameSet) {
		if (SpriteFrame* frame = m_frameSet->frame(m_frameIndex)) {
			sourceRect = frame->sourceRect();
			abchorPoint = frame->anchorPoint();
		}
	}


    Size renderSize;
    Rect renderSourceRect;
    detail::SpriteRenderFeature::makeRenderSizeAndSourceRectHelper(
        texture(), m_size, sourceRect, &renderSize, &renderSourceRect);

    //// 転送元矩形が負値ならテクスチャ全体を転送する
    //Texture* tex = texture();
    //const SizeI& texSize = (tex != nullptr) ? tex->size() : SizeI::Zero;
    //Rect renderSourceRect = m_srcRect;
    //if (renderSourceRect.width < 0 && renderSourceRect.height < 0)
    //{
    //    renderSourceRect.width = texSize.width;
    //    renderSourceRect.height = texSize.height;
    //}
    //Size renderSize = m_size;
    //if (renderSize.width < 0 && renderSize.height < 0)
    //{
    //    renderSize.width = renderSourceRect.width;
    //    renderSize.height = renderSourceRect.height;
    //}

    //renderSourceRect.x /= texSize.width;
    //renderSourceRect.width /= texSize.width;
    //renderSourceRect.y /= texSize.height;
    //renderSourceRect.height /= texSize.height;


    //context->setBlendMode(BlendMode::Alpha);
    //context->setOpacity(0.5);

    context->drawSprite(
        Matrix(), renderSize, abchorPoint, renderSourceRect, Color::White,
        SpriteBaseDirection::ZMinus, BillboardType::None, m_material);
}

} // namespace ln

