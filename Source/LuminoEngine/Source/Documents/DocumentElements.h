
#pragma once
#include <Lumino/Documents/Common.h>
#include <Lumino/UI/LayoutElement.h>
#include "../Graphics/GraphicsManager.h"

LN_NAMESPACE_BEGIN
namespace detail {
class DocumentsManager;
class Inline;
class Block;

enum class InternalTextElementType
{
	Common,
	LineBreak,
};

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

	// 現在の内容をすべて破棄して、新しい1つの Paragraph を作る
	void SetText(const StringRef& text);

private:
	DocumentsManager*		m_manager;
	List<RefPtr<Block>>		m_blockList;
};

/**
	@brief
*/
class TextElement
	: public Object
	, public ILayoutElement
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

	Brush* GetForeground() const;


	virtual void Render(const Matrix& transform, IDocumentsRenderer* renderer);

LN_PROTECTED_INTERNAL_ACCESS:
	virtual void OnFontDataChanged(const FontData& newData);

	// ILayoutElement interface
	virtual const PointF& GetLayoutPosition() const override;
	virtual Size GetLayoutSize() const override;
	virtual const ThicknessF& GetLayoutMargin() const override;
	virtual const ThicknessF& GetLayoutPadding() const override;
	virtual AlignmentAnchor GetLayoutAnchor() const override;
	virtual HAlignment GetLayoutHAlignment() const override;
	virtual VAlignment GetLayoutVAlignment() const override;
	virtual ILayoutElement* GetLayoutParent() const override;
	virtual const HAlignment* GetLayoutContentHAlignment() override;
	virtual const VAlignment* GetLayoutContentVAlignment() override;
	virtual const Size& GetLayoutDesiredSize() const override;
	virtual void SetLayoutDesiredSize(const Size& size) override;
	virtual void SetLayoutFinalLocalRect(const RectF& rect) override;
	virtual const RectF& GetLayoutFinalLocalRect() const override;
	virtual void SetLayoutFinalGlobalRect(const RectF& rect) override;
	virtual int GetVisualChildrenCount() const override;
	virtual ILayoutElement* GetVisualChild(int index) const override;
	virtual int GetLayoutColumn() const override;
	virtual int GetLayoutRow() const override;
	virtual int GetLayoutColumnSpan() const override;
	virtual int GetLayoutRowSpan() const override;

	// ILayoutElement interface
	virtual Size MeasureOverride(const Size& constraint) override;

LN_INTERNAL_ACCESS:
	DocumentsManager* GetManager() const { return m_manager; }
	virtual InternalTextElementType GetInternalTextElementType() const;
	void SetParent(TextElement* parent) { m_parent = parent; }
	TextElement* GetParent() const { return m_parent; }
	const Size& GetDesiredSize() const { return m_desiredSize; }
	const Size& GetRenderSize() const { return m_finalLocalRect.GetSize(); }

private:
	DocumentsManager*		m_manager;
	FontData				m_fontData;
	RefPtr<Brush>			m_foreground;
	bool					m_fontDataModified;

	// layout data
	PointF					m_position;
	Size					m_size;
	ThicknessF				m_margin;
	ThicknessF				m_padding;
	AlignmentAnchor			m_anchor;
	HAlignment				m_horizontalAlignment;
	VAlignment				m_verticalAlignment;
	TextElement*			m_parent;
	Size					m_desiredSize;
	RectF					m_finalLocalRect;
	RectF					m_finalGlobalRect;
	detail::GridLayoutInfo	m_gridLayoutInfo;
};

/**
	@brief	コンテンツをグループ化して段落にするために使用される。
*/
class Block
	: public TextElement
{
public:
	Block();
	virtual ~Block();
	void Initialize(DocumentsManager* manager);

	void AddChildElement(TextElement* inl);
	void ClearChildElements();

	virtual void Render(const Matrix& transform, IDocumentsRenderer* renderer);

protected:
	virtual Size MeasureOverride(const Size& constraint) override;
	virtual Size ArrangeOverride(const Size& finalSize) override;

LN_INTERNAL_ACCESS:
	const List<RefPtr<TextElement>>& GetChildElements() const { return m_childElements; }

private:
	List<RefPtr<TextElement>>	m_childElements;
};

/**
	@brief	コンテンツをグループ化して段落にするために使用される。
*/
class Paragraph
	: public Block
{
public:
	Paragraph();
	virtual ~Paragraph();
	void Initialize(DocumentsManager* manager);


private:
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
	virtual void Render(const Matrix& transform, IDocumentsRenderer* renderer) override;

	// ILayoutElement interface
	virtual Size MeasureOverride(const Size& constraint);

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


/**
	@brief
*/
class LineBreak
	: public Inline
{
public:
	LineBreak();
	virtual ~LineBreak();
	void Initialize(DocumentsManager* manager);

private:
	virtual InternalTextElementType GetInternalTextElementType() const;
};




class VisualTextElement
	: public Object
{
public:
	//TextElement*	m_element;
	int				m_documentLength;	// Document 上での TextLength

};



// 物理行
class VisualLine
	: public Object
{
public:

private:
public:	// TODO:
	List<RefPtr<VisualTextElement>>	m_visualTextElementList;
};


/**
	@brief
	@note	今は float(ブロックの横並び) などを考えない。とすると、Paragraph 単位でデータモデル組んだほうがわかりやすい。
*/
class ParagraphViewer
	: public Object
{
public:
	void SetParagraph(Paragraph* paragraph);

private:
	void RebuildVisualLineList();

	RefPtr<Paragraph>			m_paragraph;
	List<RefPtr<VisualLine>>	m_visualLineList;
};

/**
	@brief
*/
class DocumentViewer
	: public Object
{
public:

private:
};







} // namespace detail
LN_NAMESPACE_END
