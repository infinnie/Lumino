﻿
#include "../Internal.h"
#include <Lumino/Graphics/Text/Font.h>
#include <Lumino/Graphics/Text/GlyphRun.h>
#include "../GraphicsManager.h"
#include "FontManager.h"
#include "BitmapTextRenderer.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GRAPHICS_BEGIN

//==============================================================================
// RawFont
//==============================================================================

//------------------------------------------------------------------------------
RawFontPtr RawFont::GetDefaultFont()
{
	RawFontPtr font(GraphicsManager::GetInstance()->GetFontManager()->GetDefaultFont(), true);
	return font;
}

//------------------------------------------------------------------------------
RawFont::RawFont()
{
}

//------------------------------------------------------------------------------
RawFont::~RawFont()
{
}

//------------------------------------------------------------------------------
SizeI RawFont::GetTextSize(const StringRef& text)
{
	auto* r = GetManager()->GetGraphicsManager()->GetBitmapTextRenderer();
	auto* gr = r->GetTempGlyphRun();
	gr->SetFont(this);
	gr->SetText(text);
	return gr->GetRenderSize();
}

LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END
