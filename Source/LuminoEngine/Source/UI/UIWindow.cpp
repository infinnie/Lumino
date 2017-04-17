﻿
#include "Internal.h"
#include <Lumino/Graphics/DrawingContext.h>
#include <Lumino/UI/UILayoutPanel.h>
#include <Lumino/UI/UIWindow.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// UIWindow
//==============================================================================
LN_UI_TYPEINFO_IMPLEMENT(UIWindow, UIControl)

//------------------------------------------------------------------------------
RefPtr<UIWindow> UIWindow::Create()
{
	return NewObject<UIWindow>();
}

//------------------------------------------------------------------------------
UIWindow::UIWindow()
	: m_dragStartWindowPosition()
	, m_dragStartLocalPosition()
	, m_isDragging(false)
{
}

//------------------------------------------------------------------------------
UIWindow::~UIWindow()
{
}

//------------------------------------------------------------------------------
void UIWindow::Initialize()
{
	UIControl::Initialize();
}

//------------------------------------------------------------------------------
void UIWindow::OnRoutedEvent(const UIEventInfo* ev, UIEventArgs* e)
{
	if (ev == UIElement::MouseDownEvent)
	{
		if (!m_isDragging)
		{
			auto mouseEvent = static_cast<UIMouseEventArgs*>(e);
			PointF pos = mouseEvent->GetPosition(GetVisualParent());

			m_dragStartWindowPosition = GetPosition();
			m_dragStartLocalPosition = pos;
			m_isDragging = true;
			CaptureMouse();

			e->handled = true;
			return;
		}
	}
	else if (ev == UIElement::MouseUpEvent)
	{
		if (m_isDragging)
		{
			auto mouseEvent = static_cast<UIMouseEventArgs*>(e);
			PointF pos = mouseEvent->GetPosition(GetVisualParent());

			m_isDragging = false;
			ReleaseMouseCapture();

			e->handled = true;
			return;
		}
	}
	else if (ev == UIElement::MouseMoveEvent)
	{
		if (m_isDragging)
		{
			auto mouseEvent = static_cast<UIMouseEventArgs*>(e);
			Vector2 diff = mouseEvent->GetPosition(GetVisualParent()) - m_dragStartLocalPosition;

			SetPosition(m_dragStartWindowPosition + diff);
			
			

			e->handled = true;
			return;
		}
	}

	UIElement::OnRoutedEvent(ev, e);
}

//------------------------------------------------------------------------------
void UIWindow::OnLayoutPanelChanged(UILayoutPanel* newPanel)
{
	if (newPanel != nullptr)
	{
		newPanel->SetStyleSubControlName(_T("UIWindow"), _T("ContentHost"));

		//newPanel->margin = ThicknessF(0, 30, 0, 0);
	}

	UIControl::OnLayoutPanelChanged(newPanel);
}

//------------------------------------------------------------------------------
void UIWindow::OnRender(DrawingContext* g)
{
	UIControl::OnRender(g);
	g->SetBlendMode(BlendMode::Alpha);
	g->SetBrush(Brush::White);
	//g->DrawBoxBorder(
	//	RectF(10, 10, 300, 200), ThicknessF(3, 3, 3, 3), Color::Red, Color::Green, Color::Blue, Color::Cyan, 0, 10, 20, 30,
	//	Color::Gray, 5, 5, false);	// TODO:

	Color c = Color::LightSkyBlue;
	c.a = 0.25;
	g->DrawBoxBorder(
		RectF(10, 10, 300, 50), ThicknessF(1, 1, 1, 1), Color::LightSkyBlue, Color::LightSkyBlue, Color::LightSkyBlue, Color::LightSkyBlue, 30, 0, 0, 30,
		c, 5, 5, true);	// TODO:

}

LN_NAMESPACE_END
