﻿
#include "Internal.hpp"
#include <LuminoEngine/UI/UIStyle.hpp>
#include <LuminoEngine/UI/UILayoutPanel.hpp>
#include <LuminoEngine/UI/UIControl.hpp>
#include <LuminoEngine/UI/UIActiveTimer.hpp>
#include <LuminoEngine/UI/UICommand.hpp>
#include "UIManager.hpp"

namespace ln {

//==============================================================================
// UIControl

LN_OBJECT_IMPLEMENT(UIControl, UIElement) {}

UIControl::UIControl()
    : m_enabledDirectChildrenContentAlignment(true)
	, m_logicalChildren(makeCollection<Ref<UIElement>>())
{
}

void UIControl::init()
{
    UIElement::init();
    auto vsm = getVisualStateManager();
    vsm->registerState(UIVisualStates::CommonStates, UIVisualStates::Normal);
    vsm->registerState(UIVisualStates::CommonStates, UIVisualStates::MouseOver);
    vsm->registerState(UIVisualStates::CommonStates, UIVisualStates::Disabled);

	//setHorizontalAlignment(HAlignment::Stretch);
	//setVerticalAlignment(VAlignment::Stretch);

    //setLayoutPanel(makeObject<UIFrameLayout>());
}

void UIControl::onDispose(bool explicitDisposing)
{
    removeAllChildren();
	UIElement::onDispose(explicitDisposing);
}

void UIControl::setHorizontalContentAlignment(HAlignment value)
{
    m_localStyle->mainStyle()->horizontalContentAlignment = value;
}

HAlignment UIControl::horizontalContentAlignment() const
{
    return m_localStyle->mainStyle()->horizontalContentAlignment;
}

void UIControl::setVerticalContentAlignment(VAlignment value)
{
    m_localStyle->mainStyle()->verticalContentAlignment = value;
}

VAlignment UIControl::verticalContentAlignment() const
{
    return m_localStyle->mainStyle()->verticalContentAlignment;
}

void UIControl::addElement(UIElement* element)
{
	if (LN_REQUIRE(element)) return;

    // 通常は作成直後、デフォルトの Container に追加されている。
    // WPF のように別の親に追加済みであれば例外するのもありだけど、いちいち removeFromParent するのは面倒。
    if (element->logicalParent()) {
        element->removeFromLogicalParent();
    }
	
    m_logicalChildren->add(element);
    element->setLogicalParent(this);
	addVisualChild(element);

	//if (m_logicalChildrenHost) {
	//	m_logicalChildrenHost->addLayoutOwnerLogicalChild(element);
	//}
}

void UIControl::removeElement(UIElement* element)
{
    if (LN_REQUIRE(element)) return;
    if (LN_REQUIRE(element->logicalParent() == this)) return;

    //if (m_logicalChildrenHost) {
    //    m_logicalChildrenHost->removeLayoutOwnerLogicalChild(element);
    //}

    m_logicalChildren->remove(element);
    element->setLogicalParent(nullptr);
	removeVisualChild(element);
}

void UIControl::removeAllChildren()
{
	for (auto& e : *m_logicalChildren) {
		removeVisualChild(e);
	}

	m_logicalChildren->clear();
	//if (m_logicalChildrenHost) {
	//	m_logicalChildrenHost->clearLayoutOwnerLogicalChildren();
	//}
}

void UIControl::addInlineElement(UIElement* element, UIInlineLayout layout)
{
    PointI pts[] = {
        { 0, 0 }, { 1, 0 }, { 2, 0 },
        { 0, 1 }, { 1, 1 }, { 2, 1 },
        { 0, 2 }, { 1, 2 }, { 2, 2 },
    };
    element->getGridLayoutInfo()->layoutRow = pts[(int)layout].y;
    element->getGridLayoutInfo()->layoutColumn = pts[(int)layout].x;
    m_inlineElements.add(element);
    addVisualChild(element);

    if (!m_aligned3x3GridLayoutArea) {
        m_aligned3x3GridLayoutArea = makeObject<detail::UIAligned3x3GridLayoutArea>();
    }
}

//void UIControl::setLayoutPanel(UILayoutPanel* panel)
//{
//	if (m_layout != panel) {
//		m_layout = panel;
//		if (!m_layout) {
//			m_layout = makeObject<UIFrameLayout>();
//		}
//		onLayoutPanelChanged(m_layout);
//		//setLogicalChildrenHost(panel);
//	}
//}
//
//UILayoutPanel* UIControl::layoutPanel() const
//{
//	//if (m_layout)
//		return m_layout;
//	//else
//	//	return m_manager->defaultLayout();
//}
//
//UILayoutPanel* UIControl::layoutPanel() const
//{
//    return m_layout;
//    //LN_UNREACHABLE();
//    //return nullptr;//m_logicalChildrenHost;
//}

//int UIControl::getVisualChildrenCount() const
//{
//	//if (m_logicalChildrenHost) {
//	//	return 1;
//	//}
//	//else {
//		return m_logicalChildren.size();
//	//}
//}
//
//UIElement* UIControl::getVisualChild(int index) const
//{
//	//if (m_logicalChildrenHost) {
//	//	return m_logicalChildrenHost;
//	//}
//	//else {
//		return m_logicalChildren[index];
//	//}
//}
//


void UIControl::registerActiveTimer(UIActiveTimer* timer)
{
	if (LN_REQUIRE(timer)) return;
	if (LN_REQUIRE(!timer->m_owner)) return;
	m_activeTimers.add(timer);
	timer->m_owner = this;
	detail::EngineDomain::uiManager()->registerActiveTimer(timer);
}

void UIControl::unregisterActiveTimer(UIActiveTimer* timer)
{
	if (LN_REQUIRE(timer)) return;
	if (LN_REQUIRE(timer->m_owner == this)) return;
	m_activeTimers.remove(timer);
	timer->m_owner = nullptr;
	detail::EngineDomain::uiManager()->unregisterActiveTimer(timer);
}

void UIControl::addAction(UIAction* action)
{
    if (!m_actions) {
        m_actions = ln::makeList<Ref<UIAction>>();
    }
    m_actions->add(action);
}

void UIControl::onAddChild(UIElement* child)
{
    addElement(child);
}

void UIControl::onRoutedEvent(UIEventArgs* e)
{
    if (e->type() == UIEvents::MouseEnterEvent) {
        getVisualStateManager()->gotoState(UIVisualStates::MouseOver);
    }
    else if (e->type() == UIEvents::MouseLeaveEvent) {
        getVisualStateManager()->gotoState(UIVisualStates::Normal);
    }

    if (m_actions) {
        if (detail::UICommandInternal::handleCommandRoutedEvent(e, *m_actions)) {
            return;
        }
    }

    UIElement::onRoutedEvent(e);
}

Size UIControl::measureOverride(UILayoutContext* layoutContext, const Size& constraint)
{
    if (m_aligned3x3GridLayoutArea) {
        // 論理子要素の領域 (content area)
		Size childrenAreaSize = detail::LayoutHelper::UIFrameLayout_staticMeasureChildrenAreaSize(layoutContext, m_logicalChildren, constraint);//UIFrameLayout2::staticMeasureChildrenAreaSize(m_logicalChildren, constraint);
        // Inline 要素も含めた領域 (client area)
        Size clientAreaSize = m_aligned3x3GridLayoutArea->measure(layoutContext, m_inlineElements, constraint, childrenAreaSize);
        // padding, border も含めたサイズ (client は、this と clientAreaSize のうち大きい方を採用)
        return detail::LayoutHelper::measureElement(this, constraint, clientAreaSize);
    }
    else {
        //   struct ElementList : public IUIElementList {
        //       List<Ref<UIElement>>* list;
        //       virtual int getElementCount() const { return list->size(); }
        //       virtual UIElement* getElement(int i) const { return list->at(i); }
        //   } list;
        //   list.list = &m_logicalChildren;

           //UILayoutPanel* layout = layoutPanel();
           //layout->measureLayout(&list, constraint);
        //   Size desiredSize = layout->desiredSize();
        //   Size localSize = UIElement::measureOverride(constraint);
        //   return Size::max(desiredSize, localSize);

           //if (m_logicalChildrenHost) {
        //       m_logicalChildrenHost->measureLayout(constraint);
        //       Size layoutSize = m_logicalChildrenHost->desiredSize();
        //       Size localSize = UIElement::measureOverride(constraint);
        //       return Size::max(layoutSize, localSize);
           //}
           //else {
        return UIFrameLayout2::staticMeasureOverride(layoutContext, this, constraint);

        //}
    }

}

Size UIControl::arrangeOverride(UILayoutContext* layoutContext, const Size& finalSize)
{
    if (m_aligned3x3GridLayoutArea) {
        // padding, border を考慮した領域を計算
        Rect clientArea = detail::LayoutHelper::arrangeClientArea(this, finalSize);
        // Inline 要素を arrange & 論理子要素の領域 (content area) を計算
        Rect contentArea;
        m_aligned3x3GridLayoutArea->arrange(layoutContext, m_inlineElements, clientArea, &contentArea);
        // 論理子要素を arrange
		detail::LayoutHelper::UIFrameLayout_staticArrangeChildrenArea(layoutContext, this, m_logicalChildren, contentArea);
        //UIFrameLayout2::staticArrangeChildrenArea(this, m_logicalChildren, contentArea);

        return finalSize;
    }
    else {
        //UILayoutPanel* layout = layoutPanel();

     //   Rect contentSlotRect;
     //   if (m_enabledDirectChildrenContentAlignment) {
     //       detail::LayoutHelper::adjustAlignment(Rect(0, 0, finalSize), layout->desiredSize(), m_finalStyle->horizontalContentAlignment, m_finalStyle->verticalContentAlignment, &contentSlotRect);
     //   }
     //   else {
     //       contentSlotRect = Rect(0, 0, finalSize);
     //   }

     //   contentSlotRect = contentSlotRect.makeDeflate(m_finalStyle->padding);
     //   

     //   struct ElementList : public IUIElementList {
     //       List<Ref<UIElement>>* list;
     //       virtual int getElementCount() const { return list->size(); }
     //       virtual UIElement* getElement(int i) const { return list->at(i); }
     //   } list;
     //   list.list = &m_logicalChildren;

        //layout->arrangeLayout(&list, contentSlotRect);
     //   return finalSize;

     //   Rect contentSlotRect(0, 0, finalSize);
     //   detail::LayoutHelper::adjustAlignment(Rect(0, 0, finalSize), desiredSize(), m_finalStyle->horizontalContentAlignment, m_finalStyle->verticalContentAlignment, &contentSlotRect);

        ////if (m_logicalChildrenHost) {
     ////       m_logicalChildrenHost->arrangeLayout(contentSlotRect.makeDeflate(finalStyle()->padding));
        ////	return finalSize;
        ////}
        ////else {
        //	return UIFrameLayout2::staticArrangeOverride(this, contentSlotRect.getSize());
        ////}
        return UIFrameLayout2::staticArrangeOverride(layoutContext, this, finalSize);
    }
}

void UIControl::onLayoutPanelChanged(UILayoutPanel* newPanel)
{
}


//==============================================================================
// UIAligned3x3GridLayout

namespace detail {

UIAligned3x3GridLayoutArea::UIAligned3x3GridLayoutArea()
{
}

void  UIAligned3x3GridLayoutArea::init()
{
    Object::init();
}

Size UIAligned3x3GridLayoutArea::measure(UILayoutContext* layoutContext, const List<Ref<UIElement>>& inlineElements, const Size& constraint, const Size& contentDesiredSize)
{
    for (int i = 0; i < inlineElements.size(); i++)
    {
        UIElement* child = inlineElements[i];
        child->measureLayout(layoutContext, constraint);
        const Size& childDesiredSize = child->getLayoutDesiredSize();

        int row, column, rowSpan, columnSpan;
        getGridInfoHelper(child, &row, &column, &rowSpan, &columnSpan);

        // span を考慮し、この child が影響するセル範囲の 右と下 の分割線の位置を押し上げるように領域の調整を行う
        int br = std::min(std::max(row, 0), 3);
        int bc = std::min(std::max(column, 0), 3);
        int tr = std::min(std::max(row + rowSpan - 1, 0), 3);
        int tc = std::min(std::max(column + columnSpan - 1, 0), 3);
        float leftLineOffset = (br == 0) ? 0 : m_rows[br - 1].desiredLastOffset;
        float topLineOffset = (bc == 0) ? 0 : m_columns[bc - 1].desiredLastOffset;
        m_rows[tr].desiredLastOffset = std::max(m_rows[tr].desiredLastOffset, leftLineOffset + childDesiredSize.height);
        m_columns[tc].desiredLastOffset = std::max(m_columns[tr].desiredLastOffset, topLineOffset + childDesiredSize.width);
    }

    // contentSize を中央のセルとして計算する
    m_rows[1].desiredLastOffset = std::max(m_rows[1].desiredLastOffset, m_rows[0].desiredLastOffset + contentDesiredSize.height);
    m_columns[1].desiredLastOffset = std::max(m_columns[1].desiredLastOffset, m_columns[0].desiredLastOffset + contentDesiredSize.width);

    // 各セルの desiredSize を確定させる
    for (int i = 0; i < 3; i++)
    {
        float pr = (i == 0) ? 0.0f : m_rows[i - 1].desiredLastOffset;
        float pc = (i == 0) ? 0.0f : m_columns[i - 1].desiredLastOffset;

        if (m_rows[i].desiredLastOffset < pr) m_rows[i].desiredLastOffset = pr;
        if (m_columns[i].desiredLastOffset < pc) m_columns[i].desiredLastOffset = pc;

        m_rows[i].desiredSize = m_rows[i].desiredLastOffset - pr;
        m_columns[i].desiredSize = m_columns[i].desiredLastOffset - pc;
    }

    // 計算が終わると、右端と下端の次の分割線の位置がサイズとみなせる
    return Size(m_columns[2].desiredLastOffset, m_rows[2].desiredLastOffset);
}

void UIAligned3x3GridLayoutArea::arrange(UILayoutContext* layoutContext, const List<Ref<UIElement>>& inlineElements, const Rect& finalArea, Rect* outActualContentRect)
{
    LN_CHECK(outActualContentRect);

    // 両端は Auto 扱い。desiredSize を使ってそのまま確定。
    m_rows[0].actualSize = m_rows[0].desiredSize;
    m_rows[2].actualSize = m_rows[2].desiredSize;
    m_columns[0].actualSize = m_columns[0].desiredSize;
    m_columns[2].actualSize = m_columns[2].desiredSize;

    // 中心は Ratio. ただ1つしかないので残った領域をそのまま使う
    m_rows[1].actualSize = finalArea.height - m_rows[0].actualSize - m_rows[2].actualSize;
    m_columns[1].actualSize = finalArea.width - m_columns[0].actualSize - m_columns[2].actualSize;

    // actualOffset を確定させる
    Vector2 offset(0.0f, 0.0f);
    for (int i = 0; i < 3; i++)
    {
        m_rows[i].actualOffset = offset.y;
        m_columns[i].actualOffset = offset.x;
        offset.y += m_rows[i].actualSize;
        offset.x += m_columns[i].actualSize;
    }

    // arrange
    for (int i = 0; i < inlineElements.size(); i++)
    {
        UIElement* child = inlineElements[i];
        int row, column, rowSpan, columnSpan;
        getGridInfoHelper(child, &row, &column, &rowSpan, &columnSpan);

        Rect rect(finalArea.x + m_columns[column].actualOffset, finalArea.y + m_rows[row].actualOffset, 0, 0);
        for (int i = 0; i < rowSpan; i++) {
            rect.height += m_rows[row + i].actualSize;
        }
        for (int i = 0; i < columnSpan; i++) {
            rect.width += m_columns[column + i].actualSize;
        }

        child->arrangeLayout(layoutContext, rect);
    }

    // content area (呼び出し側でレイアウトするのに使う)
    outActualContentRect->x = finalArea.x + m_columns[1].actualOffset;
    outActualContentRect->y = finalArea.y + m_rows[1].actualOffset;
    outActualContentRect->width = m_columns[1].actualSize;
    outActualContentRect->height = m_rows[1].actualSize;
}

void UIAligned3x3GridLayoutArea::getGridInfoHelper(UIElement* element, int* row, int* column, int* rowSpan, int* columnSpan) const
{
    auto* info = element->m_gridLayoutInfo.get();
    if (info) {
        *row = std::min(std::max(info->layoutRow, 0), (int)m_rows.size());
        *column = std::min(std::max(info->layoutColumn, 0), (int)m_columns.size());
        *rowSpan = ((*row) + info->layoutRowSpan > (m_rows.size() + 1)) ? (m_rows.size() + 1) - (*row) : info->layoutRowSpan;
        *columnSpan = ((*column) + info->layoutColumnSpan > (m_columns.size() + 1)) ? (m_columns.size() + 1) - (*column) : info->layoutColumnSpan;
    }
    else {
        *row = 0;
        *column = 0;
        *rowSpan = 1;
        *columnSpan = 1;
    }
}

} // namespace detail


} // namespace ln

