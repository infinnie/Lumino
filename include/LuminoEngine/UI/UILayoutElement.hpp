﻿#pragma once
#include "../Graphics/GeometryStructs.hpp"
#include "../Graphics/ColorStructs.hpp"
#include "../Rendering/Common.hpp"

namespace ln {
class Font;
class UIStyle;
namespace detail { class LayoutHelper; }


enum class UIFontWeight
{
	Normal,
	Bold,
};

enum class UIFontStyle
{
	Normal,
	Italic,
};

/** 縦方向の表示位置を示します。*/
enum class VAlignment
{
	Top = 0,			/**< 子要素を、親のレイアウト スロットの上端に揃えて配置します。*/
	Center,				/**< 子要素を、親のレイアウト スロットの中央に揃えて配置します。*/
	Bottom,				/**< 子要素を、親のレイアウト スロットの下端に揃えて配置します。*/
	Stretch,			/**< 子要素を、親のレイアウト スロット全体に引き伸ばします。*/
};

/** 横方向の表示位置を示します。*/
enum class HAlignment
{
	Left = 0,			/**< 子要素を、親のレイアウト スロットの左側に揃えて配置します。*/
	Center,				/**< 子要素を、親のレイアウト スロットの中央に揃えて配置します。*/
	Right,				/**< 子要素を、親のレイアウト スロットの右側に揃えて配置します。*/
	Stretch,			/**< 子要素を、親のレイアウト スロット全体に引き伸ばします。*/
};


namespace detail
{
//struct UIInheritStyleAttribute
//{
//	Color textColor;
//	String fontFamily;
//	float fontSize;
//	UIFontStyle fontStyle;
//};

// どんな Element でも持つ一般的なスタイル値。
// メモリ消費を抑えるため、UIStyleAttribute は使わないようにしている。
struct StyleData
{
	UIStyle* sourceLocalStyle;	// 以下のデータの生成元となったローカスのスタイル

	// layout
	Thickness margin;
	Thickness padding;
	HAlignment horizontalAlignment;
	VAlignment verticalAlignment;
	float minWidth;
	float minHeight;
	float maxWidth;
	float maxHeight;

	// layout transform
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
	Vector3 centerPoint;

	// text
	Color textColor;
	Ref<Font> font;
	//String fontFamily;
	//float fontSize;
	//UIFontWeight fontWeight;
	//UIFontStyle fontStyle;

	// render effects
	bool visible;
	BlendMode blendMode;

	float opacity;
	Color colorScale;
	Color blendColor;
	ToneF tone;

	// TODO: 今後サブクラスごとにスタイルを追加する場合は、ここに map を設ける
};
}

class UILayoutElement
	: public Object
{
public:	// TODO: internal
	void updateLayout(const Rect& parentFinalGlobalRect);

	void setWidth(float value) { m_layoutSize.width = value; }
	float width() const { return m_layoutSize.width; }

	void setHeight(float value) { m_layoutSize.height = value; }
	float height() const { return m_layoutSize.height; }


	/** レイアウト処理の測定パスの実行中にこの要素が計算したサイズを取得します。この値は子要素が親要素へ要求する、子要素自身の最低サイズです。*/
	const Size& desiredSize() const { return m_desiredSize; }

	const Rect& finalGlobalRect() const { return m_finalGlobalRect; }

protected:
	UILayoutElement();
	virtual ~UILayoutElement();
	void init(const detail::StyleData* finalStyle);

	virtual Size measureOverride(const Size& constraint);
	virtual Size arrangeOverride(const Size& finalSize);

	//virtual int getChildLayoutItemCount() const = 0;
	//virtual ILayoutElement* getChildLayoutItem(int index) const = 0;

private:
	void measureLayout(const Size& availableSize);
	void arrangeLayout(const Rect& finalLocalRect);
	void updateFinalRects(const Rect& parentGlobalRect);

	const Size& getLayoutSize() const { return m_layoutSize; }
	void setLayoutDesiredSize(const Size& size) { m_desiredSize = size; }
	const Size& getLayoutDesiredSize() const { return m_desiredSize; }
	const Thickness& getLayoutMargin() const;
	const Thickness& getLayoutPadding() const;
	HAlignment getLayoutHAlignment() const;
	VAlignment getLayoutVAlignment() const;
	void getLayoutMinMaxInfo(Size* outMin, Size* outMax) const;

	void setLayoutFinalLocalRect(const Rect& rect) { m_finalLocalRect = rect; }
	const Rect& getLayoutFinalLocalRect() const { return m_finalLocalRect; }
	void setLayoutFinalGlobalRect(const Rect& rect) { m_finalGlobalRect = rect; }

	Size m_layoutSize;
	const detail::StyleData* m_finalStyle;
	Size m_desiredSize;
	Rect m_finalLocalRect;
	Rect m_finalGlobalRect;

	friend class detail::LayoutHelper;
};

class ILayoutPanel
{
protected:
    ILayoutPanel();

private:
};

namespace detail {

class LayoutHelper
{
public:

	// 単純に element のサイズによった measure を行う。measureOverride() の中で使用することを想定している。
	static Size measureElement(UILayoutElement* element, const Size& constraint);

	//static void forEachVisualChildren(UIElement* element, std::function<void(UIElement* child)> func)
	//{
	//	int count = element->getVisualChildrenCount();
	//	for (int i = 0; i < count; ++i)
	//	{
	//		func(element->getVisualChild(i));
	//	}
	//}

	// widthNan : ユーザーが希望するサイズを指定しているか
	static void adjustHorizontalAlignment(const Size& areaSize, const Size& desiredSize, bool widthNan, HAlignment align, Rect* outRect)
	{
		switch (align)
		{
		case HAlignment::Left:
			outRect->x = 0;
			outRect->width = desiredSize.width;
			break;
		case HAlignment::Center:
			outRect->x = (areaSize.width - desiredSize.width) / 2;
			outRect->width = desiredSize.width;
			break;
		case HAlignment::Right:
			outRect->x = areaSize.width - desiredSize.width;
			outRect->width = desiredSize.width;
			break;
		case HAlignment::Stretch:
			if (widthNan)
			{
				outRect->x = 0;
				outRect->width = areaSize.width;
			}
			else
			{
				outRect->x = (areaSize.width - desiredSize.width) / 2;
				outRect->width = desiredSize.width;
			}

			//outRect->x = 0;
			//outRect->width = areaSize.width;
			break;
		}
	}

	static void adjustVerticalAlignment(const Size& areaSize, const Size& desiredSize, bool heightNan, VAlignment align, Rect* outRect)
	{
		switch (align)
		{
		case VAlignment::Top:
			outRect->y = 0;
			outRect->height = desiredSize.height;
			break;
		case VAlignment::Center:
			outRect->y = (areaSize.height - desiredSize.height) / 2;
			outRect->height = desiredSize.height;
			break;
		case VAlignment::Bottom:
			outRect->y = areaSize.height - desiredSize.height;
			outRect->height = desiredSize.height;
			break;
		case VAlignment::Stretch:
			if (heightNan)
			{
				outRect->y = 0;
				outRect->height = areaSize.height;
			}
			else
			{
				outRect->y = (areaSize.height - desiredSize.height) / 2;
				outRect->height = desiredSize.height;
			}

			break;
		}
	}
};

} // namespace detail
} // namespace ln

