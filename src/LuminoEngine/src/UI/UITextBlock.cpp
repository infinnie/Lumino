﻿
#include "Internal.hpp"
#include <LuminoEngine/Engine/VMProperty.hpp>

#include <LuminoEngine/Font/Font.hpp>
#include <LuminoEngine/Platform/PlatformWindow.hpp>
#include <LuminoEngine/UI/UIStyle.hpp>
#include <LuminoEngine/UI/UIRenderingContext.hpp>
#include <LuminoEngine/UI/UIFrameWindow.hpp>
#include <LuminoEngine/UI/UITextBlock.hpp>

#include "../Font/TextLayoutEngine.hpp"
#include "../Font/FontManager.hpp"
#include "UIStyleInstance.hpp"

// TODO: Test
#include <LuminoEngine/Graphics/Texture.hpp>
#include <LuminoEngine/Rendering/Material.hpp>


namespace ln {

//==============================================================================
// UITextBlock
LN_OBJECT_IMPLEMENT(UITextBlock, UIElement) {
	 typeInfo->registerViewProperty(makeRef<ViewPropertyInfo>(TypeInfo::getTypeInfo<String>(), "text", LN_MAKE_VIEW_PROPERTY_ACCESSOR(UITextBlock, String, text, setText)));
}


ViewProperty* UITextBlock::getViewProperty(StringRef name)
{
	ViewPropertyInfo* info = TypeInfo::getTypeInfo(this)->findViewProperty(name);
	auto itr = std::find_if(m_viewProperties.begin(), m_viewProperties.end(), [&](auto& x) { return x->m_info == info; });
	if (itr != m_viewProperties.end())
		return (*itr);
	else {
		auto prop = makeRef<ViewProperty>();
		prop->m_owner = this;
		prop->m_info = info;
		m_viewProperties.push_back(prop);
		return prop;
	}
}


Ref<UITextBlock> UITextBlock::create()
{
    return makeObject<UITextBlock>();
}

Ref<UITextBlock> UITextBlock::create(const StringRef& text)
{
    return makeObject<UITextBlock>(text);
}

UITextBlock::UITextBlock()
{
}

void UITextBlock::init()
{
    UIElement::init();

    // Alignment は Center をデフォルトにしてみる。
    // Web だと Top-Left だが、これは下方向のサイズ上限がないっていうのと、活版印刷を意識しているため…と思ってる。
    //
    // WPF も Top-Left がデフォルト。これは Desktop アプリの文化かな。
    // 実際のところ、これまでいろいろなアプリ作ってきたけど Center-Left が一番よく使う。次に Top-Center.
    //
    // モバイル・タブレット系の Framework では Center-Center が多い。
    // Lumino は今後モバイル・タブレット系に寄せていきたいところ。
    // 
    // また、デフォルトが Top-Left だと表示したときぱっと目に入りづらいことがあるので、デバッグや導入的な視点からちょっとマイナスかも。
    //setHAlignment(UIHAlignment::Center);
    //setVAlignment(UIVAlignment::Center);

    // [2020/5/3] ↑やっぱりやめ。
    // 実際に UI 作ってみて思ったことだけど、センタリングしたいっていう機会よりも左寄せのほうが圧倒的に多い。
    // チュートリアル用に視認しやすさを出したとしても、実際に使うときに煩わしいのはちょっと…。
    setHAlignment(UIHAlignment::Left);
    setVAlignment(UIVAlignment::Top);

    setBlendMode(BlendMode::Alpha);

    //attemptAddToPrimaryElement();
}

void UITextBlock::init(const StringRef& text)
{
    init();
    setText(text);
}

void UITextBlock::setText(const StringRef& value)
{
	m_text = value;
}

Size UITextBlock::measureOverride(UILayoutContext* layoutContext, const Size& constraint)
{
	// TODO: LayoutContext みたいなのを作って、それから dpi scale うけとるようにしてみよう
	float scale = 1.0f;
	if (UIFrameWindow* w = static_cast<UIFrameWindow*>(getFrameWindow())) {
		scale = w->platformWindow()->dpiFactor();
	}

    // TODO: LayoutContext
    detail::FontGlobalMetrics gm;
    auto fc = detail::FontHelper::resolveFontCore(finalStyle()->font, scale);
    fc->getGlobalMetrics(&gm);

    Size contentSize = finalStyle()->font->measureRenderSize(m_text, scale);
    m_textSize = Size(contentSize.width, gm.lineSpace);

    Size baseSize = layoutContext->makeDesiredSize(this, m_textSize);

	return baseSize;
}

void UITextBlock::onRender(UIRenderingContext* context)
{
    Rect clientArea = detail::LayoutHelper::arrangeClientArea(this, Rect(0, 0, actualSize()));

    UIHAlignment ha = m_finalStyle->horizontalContentAlignment == UIHAlignment::Stretch ? UIHAlignment::Center : m_finalStyle->horizontalContentAlignment;
    UIVAlignment va = m_finalStyle->verticalContentAlignment == UIVAlignment::Stretch ? UIVAlignment::Center : m_finalStyle->verticalContentAlignment;
    Rect renderRect;
    detail::LayoutHelper::adjustHorizontalAlignment(clientArea.getSize(), m_textSize, 0.0f, ha, &renderRect);
    detail::LayoutHelper::adjustVerticalAlignment(clientArea.getSize(), m_textSize, 0.0f, va, &renderRect);

    //auto a = m_finalStyle->horizontalContentAlignment;

    //{
    //    auto tex = makeObject<Texture2D>(u"D:/Proj/LN/HC1/Assets/Windowskin/window.png");
    //    auto mat = Material::create(tex);
    //    context->drawBoxBackground(Rect(10, 20, 100, 200), Thickness(16), CornerRadius(), BrushImageDrawMode::BorderFrame, mat, Rect(64, 0, 64, 64));
    //}
    context->setTransfrom(Matrix::makeTranslation(clientArea.x + renderRect.x, clientArea.y + renderRect.y, 0.0f));

    Color color = m_finalStyle->textColor;
    if (!isEnabled()) {
        color.a = 0.5;
    }
    context->setTextColor(color);

    context->drawText(m_text);
}

} // namespace ln

