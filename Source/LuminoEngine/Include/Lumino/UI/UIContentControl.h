
#pragma once
#include "UIControl.h"

LN_NAMESPACE_BEGIN

/**
	@brief		
	@details	VerticalContentAlignment と HorizontalContentAlignment の初期値はそれぞれ Stretch です。
*/
class UIContentControl
	: public UIControl
{
	LN_OBJECT;
public:

	void setContent(UIElement* element);

protected:
	UIContentControl();
	virtual ~UIContentControl();
	void initialize();
	//virtual void onUpdatingLayout() override;


private:
	UIElement*	m_contentElement;
	bool		m_contentElementModified;
};

LN_NAMESPACE_END
