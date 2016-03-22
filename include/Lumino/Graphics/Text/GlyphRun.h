﻿
#pragma once
#include "../Common.h"
#include "Font.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GRAPHICS_BEGIN
class TextLayoutEngine;	// TODO: detail

// TODO: detail
struct TextLayoutResultItem
{
	UTF32	Char;
	FontGlyphLocation	Location;
};

struct TextLayoutResult
{
	Size						AreaSize;
	Array<TextLayoutResultItem>	Items;
};

/**
	@brief	同じ描画スタイルを適用できる一連のグリフを表します。
*/
class GlyphRun
	: public Object
{
public:
	void SetFont(Font* font);
	Font* GetFont() const;
	void SetText(const StringRef& text);
	const Size& GetRenderSize() const { return m_glyphData.AreaSize; }
	void SetTextAlignment(TextAlignment align);
	int GetStrokeSize() const;

LN_INTERNAL_ACCESS:
	GlyphRun();
	virtual ~GlyphRun();
	void Initialize(GraphicsManager* manager);
	const Array<TextLayoutResultItem>& GetLayoutItems() const;
	Internal::FontGlyphTextureCache* LookupFontGlyphTextureCache();
	const UTF32* GetText() const { return m_utf32Text.c_str(); }
	int GetTextLength() const { return m_utf32Text.GetLength(); }

#if 0
public:
	void Update(const UTF32* text, int length);

private:
	void AttachGlyphTextureCache(Internal::FontGlyphTextureCache* cache);
	Internal::FontGlyphTextureCache*	m_glyphTextureCache;
#endif

private:
	void UpdateTextLayoutItem();

	friend class Helper;
	GraphicsManager*					m_manager;
	GenericStringBuilderCore<UTF32>		m_utf32Text;
	TextLayoutResult					m_glyphData;
	TextLayoutEngine*					m_layoutEngine;	// TODO: detail にして new しない
	Internal::FontGlyphTextureCache*	m_glyphTextureCache;
};

LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END