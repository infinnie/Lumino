
#include "Internal.h"
#include <Lumino/Rendering/DrawingContext.h>
#include <Lumino/UI/UIStyle.h>
#include <Lumino/UI/UITextBlock.h>
#include <Lumino/UI/UIListBox.h>
#include <Lumino/UI/UILayoutPanel.h>	// TODO: stackpanel にするかも
#include "UIManager.h"

LN_NAMESPACE_BEGIN

//==============================================================================
// UIListBoxItem
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(UIListBoxItem, UIControl)

//------------------------------------------------------------------------------
UIListBoxItem::UIListBoxItem()
{
}

//------------------------------------------------------------------------------
UIListBoxItem::~UIListBoxItem()
{
}

//------------------------------------------------------------------------------
void UIListBoxItem::initialize()
{
	UIControl::initialize();
	setHContentAlignment(HAlignment::Left);
	setHAlignment(HAlignment::Stretch);
	goToVisualState(UIVisualStates::NormalState);

	// TODO:
	setMinHeight(16);

	//setHitTestVisible(true);
}

void UIListBoxItem::onRender(DrawingContext* g)
{
	UIControl::onRender(g);
	
	g->drawText_(_T("t"), PointF());
	//g->drawRectangle(Rect(0, 0, 1, 1));

	g->setBrush(SolidColorBrush::Blue);
	auto rc = Rect(m_finalGlobalRect.getTopLeft(), m_finalLocalRect.getSize());
	rc.x = 300;
	g->drawBoxBackground(rc, m_localStyle->cornerRadius.get());



	//if (m_localStyle->background.get() != nullptr)
	//{
	//	printf("%f\n", m_localStyle->background.get()->getColor().a);
	//}
}

//==============================================================================
// UIListBox
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(UIListBox, UIControl)

const String UIListBox::NormalState = _T("Normal");

//------------------------------------------------------------------------------
UIListBoxPtr UIListBox::create()
{
	auto ptr = UIListBoxPtr::makeRef();
	ptr->initialize();
	return ptr;
}

//------------------------------------------------------------------------------
UIListBox::UIListBox()
{
}

//------------------------------------------------------------------------------
UIListBox::~UIListBox()
{
}

//------------------------------------------------------------------------------
void UIListBox::initialize()
{
	UIControl::initialize();
	setHContentAlignment(HAlignment::Stretch);

	auto panel = RefPtr<UIStackPanel>::makeRef();
	panel->initialize();
	panel->setHAlignment(HAlignment::Stretch);
	panel->setVAlignment(VAlignment::Stretch);
	setLayoutPanel(panel);
	goToVisualState(NormalState);
}

//------------------------------------------------------------------------------
UIListBoxItemPtr UIListBox::addTextItem(const String& text)
{
	//auto textBlock = RefPtr<UITextBlock>::makeRef();
	//textBlock->initialize();
	//textBlock->setText(text);
	//return addItem(textBlock);

	auto listItem = RefPtr<UIListBoxItem>::makeRef();
	listItem->initialize();
	listItem->setSize(Size(100, 20));
	//listItem->setBackground(Brush::Blue);
	addChild(listItem);
	return listItem;
}

//------------------------------------------------------------------------------
UIListBoxItemPtr UIListBox::addItem(UIElement* item)
{
	if (LN_CHECK_ARG(item != nullptr)) return nullptr;

	// 受け取った item を UIListBoxItem でラップして、UIListBoxItem をリストに入れる
	auto listItem = RefPtr<UIListBoxItem>::makeRef();
	listItem->initialize();
	listItem->addChild(item);
	addChild(listItem);
	return listItem;
}

LN_NAMESPACE_END
