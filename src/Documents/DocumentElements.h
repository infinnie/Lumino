
#pragma once
#include <Lumino/Documents/Common.h>
#include "../Graphics/GraphicsManager.h"

LN_NAMESPACE_BEGIN
namespace detail {
class DocumentsManager;

/**
	@brief
*/
class Document
	: public Object
{
public:
	Document();
	virtual ~Document();
	void Initialize(DocumentsManager* manager);

private:
	DocumentsManager*	m_manager;
};

/**
	@brief
*/
class TextElement
	: public Object
{
public:
	TextElement();
	virtual ~TextElement();
	void Initialize(DocumentsManager* manager);

	/** フォントファミリ名を設定します。*/
	void SetFontFamily(const StringRef& value) { m_fontData.Family = value; m_fontDataModified = true; }

	/** フォントファミリ名を取得します。*/
	const String& GetFontFamily() const { return m_fontData.Family; }

	/** フォントサイズを設定します。*/
	void SetFontSize(int value) { m_fontData.Size = value; m_fontDataModified = true; }

	/** フォントサイズを取得します。*/
	int GetFontSize() const { return m_fontData.Size; }

	/** フォントの太字有無を設定します。*/
	void SetFontBold(bool value) { m_fontData.IsBold = value; m_fontDataModified = true; }

	/** フォントの太字有無を取得します。*/
	bool IsFontBold() const { return m_fontData.IsBold; }

	/** フォントのイタリック体有無を設定します。*/
	void SetFontItalic(bool value) { m_fontData.IsItalic = value; m_fontDataModified = true; }

	/** フォントのイタリック体有無を取得します。*/
	bool IsFontItalic() const { return m_fontData.IsItalic; }

	/** フォントのアンチエイリアス有無を設定します。*/
	void SetFontAntiAlias(bool value) { m_fontData.IsAntiAlias = value; m_fontDataModified = true; }

	/** フォントのアンチエイリアス有無を取得します。*/
	bool IsFontAntiAlias() const { return m_fontData.IsAntiAlias; }

protected:
	virtual void OnFontDataChanged(const FontData& newData);
	virtual SizeF Measure();
	virtual void Render(IDocumentsRenderer* renderer);

LN_INTERNAL_ACCESS:
	DocumentsManager* GetManager() const { return m_manager; }

private:
	DocumentsManager*	m_manager;
	FontData			m_fontData;
	bool				m_fontDataModified;
};

/**
	@brief	コンテンツをグループ化して段落にするために使用される。
*/
class Paragraph
	: public TextElement
{
public:
	Paragraph();
	virtual ~Paragraph();
	void Initialize(DocumentsManager* manager);

private:
	// Inline List
	
};

/**
	@brief
*/
class Inline
	: public TextElement
{
public:
	Inline();
	virtual ~Inline();
	void Initialize(DocumentsManager* manager);

private:
};

/**
	@brief
*/
class Run
	: public Inline
{
public:
	Run();
	virtual ~Run();
	void Initialize(DocumentsManager* manager);

	void SetText(const StringRef& text);

protected:
	// TextElement interface
	virtual void OnFontDataChanged(const FontData& newData) override;
	virtual SizeF Measure() override;
	virtual void Render(IDocumentsRenderer* renderer) override;

private:
	GenericStringBuilderCore<UTF32>	m_text;
	RefPtr<GlyphRun>				m_glyphRun;
};

/**
	@brief
*/
class Span
	: public Inline
{
public:
	Span();
	virtual ~Span();
	void Initialize(DocumentsManager* manager);

private:
	// Inline List
};

} // namespace detail
LN_NAMESPACE_END
