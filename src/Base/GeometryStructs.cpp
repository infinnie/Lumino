﻿
#include "../Internal.h"
#include <Lumino/Base/GeometryStructs.h>

LN_NAMESPACE_BEGIN

//==============================================================================
// Point
//==============================================================================
const Point	Point::Zero(0, 0);

//==============================================================================
// PointF
//==============================================================================
const PointF PointF::Zero(0, 0);

//==============================================================================
// SizeI
//==============================================================================
const SizeI	SizeI::Zero(0, 0);

//==============================================================================
// SizeF
//==============================================================================
const SizeF	SizeF::Zero(0, 0);
const SizeF	SizeF::NaN(NAN, NAN);
const SizeF SizeF::MaxValue(FLT_MAX, FLT_MAX);

//------------------------------------------------------------------------------
SizeF SizeF::Max(const SizeF& size1, const SizeF& size2)
{
	return SizeF(
		(size1.width > size2.width) ? size1.width : size2.width,
		(size1.height > size2.height) ? size1.height : size2.height);
}

//==============================================================================
// Rect
//==============================================================================
const Rect	Rect::Zero(0, 0, 0, 0);
const Rect	Rect::Empty(0, 0, -1, -1);

//==============================================================================
// RectF
//==============================================================================
const RectF	RectF::Zero(0, 0, 0, 0);
const RectF	RectF::Empty(0, 0, -1, -1);

//==============================================================================
// Box32
//==============================================================================
const Box32 Box32::Zero(0, 0, 0, 0, 0, 0);

//==============================================================================
// ThicknessF
//==============================================================================
const ThicknessF	ThicknessF::Zero(0, 0, 0, 0);

LN_NAMESPACE_END
