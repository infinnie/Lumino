
#pragma once
#include "UITextElement.h"
#include "UIControl.h"

LN_NAMESPACE_BEGIN
namespace tr { class Document; }
namespace tr { class DocumentView; }
class UITextBox;
using UITextBoxPtr = RefPtr<UITextBox>;
class UITextArea;
class UISimpleTextArea;

/**
	@brief		
*/
class UITextBox
	: public UITextElement
{
	LN_UI_TYPEINFO_DECLARE();
public:

	static UITextBoxPtr Create();

public:
	void SetText(const StringRef& text);

	virtual bool IsFocusable() const;

protected:
	virtual void OnMouseDown(UIMouseEventArgs* e) override;
	virtual void OnKeyDown(UIKeyEventArgs* e) override;
	virtual void OnTextInput(UIKeyEventArgs* e) override;

LN_PROTECTED_INTERNAL_ACCESS:
	UITextBox();
	virtual ~UITextBox();
	void Initialize();

	virtual Size MeasureOverride(const Size& availableSize) override;
	virtual Size ArrangeOverride(const Size& finalSize) override;
	virtual void OnRender(DrawingContext* g) override;

private:
	//RefPtr<tr::Document>		m_document;
	//RefPtr<tr::DocumentView>	m_documentView;
	RefPtr<UITextArea>			m_textArea;
};




/**
	@brief		
*/
class UITextField
	: public UIControl
{
	LN_UI_TYPEINFO_DECLARE();
public:
	static RefPtr<UITextField> Create();

public:
	void SetText(const StringRef& text);

	virtual bool IsFocusable() const;

protected:

LN_PROTECTED_INTERNAL_ACCESS:
	UITextField();
	virtual ~UITextField();
	void Initialize();

	virtual Size MeasureOverride(const Size& availableSize) override;
	virtual Size ArrangeOverride(const Size& finalSize) override;

private:
	RefPtr<UISimpleTextArea>			m_textArea;
};

LN_NAMESPACE_END
