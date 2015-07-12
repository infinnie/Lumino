
#include "../../Internal.h"
#include <Lumino/Property.h>
#include <Lumino/GUI/Controls/Grid.h>

namespace Lumino
{
namespace GUI
{

//=============================================================================
// ColumnDefinition
//=============================================================================
LN_CORE_OBJECT_TYPE_INFO_IMPL(ColumnDefinition);
LN_UI_ELEMENT_SUBCLASS_IMPL(ColumnDefinition);

const PropertyID	ColumnDefinition::WidthProperty(_T("Width"));
const PropertyID	ColumnDefinition::MinWidthProperty(_T("MinWidth"));
const PropertyID	ColumnDefinition::MaxWidthProperty(_T("MaxWidth"));

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ColumnDefinition::ColumnDefinition(GUIManager* manager)
	: ContentElement(manager)
	, m_width(std::numeric_limits<float>::infinity())
	, m_minWidth(0)
	, m_maxWidth(FLT_MAX)
	, m_actualWidth(0)
	, m_actualOffsetX(0)
	, m_elementGroup()
	, m_desiredWidth(0)
{
	// Register property
	LN_DEFINE_PROPERTY(ColumnDefinition, float, WidthProperty, &ColumnDefinition::SetWidth, &ColumnDefinition::GetWidth, std::numeric_limits<float>::infinity());
	LN_DEFINE_PROPERTY(ColumnDefinition, float, MinWidthProperty, &ColumnDefinition::SetMinWidth, &ColumnDefinition::GetMinWidth, 0.0f);
	LN_DEFINE_PROPERTY(ColumnDefinition, float, MaxWidthProperty, &ColumnDefinition::SetMaxWidth, &ColumnDefinition::GetMaxWidth, FLT_MAX);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ColumnDefinition::~ColumnDefinition()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
float ColumnDefinition::GetAvailableDesiredWidth() const
{
	if (IsAuto()) {
		return m_desiredWidth;
	}
	else if (IsPixel()) {
		return Math::Clamp(m_width, m_minWidth, m_maxWidth);
	}
	else { //if (IsStar()) {
		return 0;
	}
}

//=============================================================================
// RowDefinition
//=============================================================================
LN_CORE_OBJECT_TYPE_INFO_IMPL(RowDefinition);
LN_UI_ELEMENT_SUBCLASS_IMPL(RowDefinition);

const PropertyID	RowDefinition::HeightProperty(_T("Height"));
const PropertyID	RowDefinition::MinHeightProperty(_T("MinHeight"));
const PropertyID	RowDefinition::MaxHeightProperty(_T("MaxHeight"));

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RowDefinition::RowDefinition(GUIManager* manager)
	: ContentElement(manager)
	, m_height(std::numeric_limits<float>::infinity())
	, m_minHeight(0)
	, m_maxHeight(FLT_MAX)
	, m_actualHeight(0)
	, m_actualOffsetY(0)
	, m_elementGroup()
	, m_desiredHeight(0)
{
	// Register property
	LN_DEFINE_PROPERTY(RowDefinition, float, HeightProperty, &RowDefinition::SetHeight, &RowDefinition::GetHeight, std::numeric_limits<float>::infinity());
	LN_DEFINE_PROPERTY(RowDefinition, float, MinHeightProperty, &RowDefinition::SetMinHeight, &RowDefinition::GetMinHeight, 0.0f);
	LN_DEFINE_PROPERTY(RowDefinition, float, MaxHeightProperty, &RowDefinition::SetMaxHeight, &RowDefinition::GetMaxHeight, FLT_MAX);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
RowDefinition::~RowDefinition()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
float RowDefinition::GetAvailableDesiredHeight() const
{
	if (IsAuto()) {
		return m_desiredHeight;
	}
	else if (IsPixel()) {
		return Math::Clamp(m_height, m_minHeight, m_maxHeight);
	}
	else { //if (IsStar()) {
		return 0;
	}
}

//=============================================================================
// Grid
//=============================================================================
LN_CORE_OBJECT_TYPE_INFO_IMPL(Grid);
LN_UI_ELEMENT_SUBCLASS_IMPL(Grid);

const PropertyID	Grid::ColumnDefinitionsProperty(_T("ColumnDefinitions"));
const PropertyID	Grid::RowDefinitionsProperty(_T("RowDefinitions"));

const AttachedProperty*	Grid::ColumnProperty = NULL;
AttachedProperty*	Grid::ColumnSpanProperty = NULL;
AttachedProperty*	Grid::RowProperty = NULL;
AttachedProperty*	Grid::RowSpanProperty = NULL;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Grid::Grid(GUIManager* manager)
	: Panel(manager)
{
	// Register attached property
	LN_DEFINE_ATTACHED_PROPERTY(ColumnProperty,		"Column",		int, Grid, 0);
	LN_DEFINE_ATTACHED_PROPERTY(ColumnSpanProperty,	"ColumnSpan",	int, Grid, 0);
	LN_DEFINE_ATTACHED_PROPERTY(RowProperty,		"Row",			int, Grid, 0);
	LN_DEFINE_ATTACHED_PROPERTY(RowSpanProperty,	"RowSpan",		int, Grid, 0);

	m_columnDefinitionList.Attach(LN_NEW ColumnDefinitionList());
	m_rowDefinitionList.Attach(LN_NEW RowDefinitionList());
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
Grid::~Grid()
{
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Grid::MeasureLayout(const SizeF& availableSize)
{
	for (auto col : *m_columnDefinitionList) { col->m_elementGroup.Clear(); col->m_desiredWidth = 0; }
	for (auto row : *m_rowDefinitionList)	 { row->m_elementGroup.Clear(); row->m_desiredHeight = 0; }

	for (auto child : *m_children)
	{
		// 子要素を行と列ごとにグループ化する
		//		- 範囲外のインデックスが指定されていたら 0 にする。
		int colIdx = Grid::GetColumn(child);	// TODO: このプロパティ検索がボトルネックになるかも
		int rowIdx = Grid::GetRow(child);
		colIdx = m_columnDefinitionList->CheckValidIndex(colIdx) ? colIdx : 0;
		rowIdx = m_rowDefinitionList->CheckValidIndex(rowIdx)    ? rowIdx : 0;

		auto col = m_columnDefinitionList->GetAt(colIdx);
		auto row = m_rowDefinitionList->GetAt(colIdx);

		col->m_elementGroup.Add(child);
		row->m_elementGroup.Add(child);

		// 子要素の DesiredSize (最低サイズ) を測るのは、セルのサイズ指定が "Auto" の時だけでよい。
		if (col->IsAuto() || row->IsAuto())
		{
			child->MeasureLayout(availableSize);	// TODO: Measuer は常にやったほうが良いのか確認しておいたほうが良いかも

			if (col->IsAuto() && Grid::GetColumnSpan(child) <= 1) {	// span が 2 以上の要素はサイズ集計に考慮しない
				col->m_desiredWidth = std::max(col->m_desiredWidth, child->GetDesiredSize().Width);
			}
			if (row->IsAuto() && Grid::GetRowSpan(child) <= 1) {	// span が 2 以上の要素はサイズ集計に考慮しない
				row->m_desiredHeight = std::max(row->m_desiredHeight, child->GetDesiredSize().Height);
			}
		}
	}

	// 各セルの DesiredSize を集計して、Grid 全体の DesiredSize を求める。
	SizeF desiredSize = SizeF::Zero;
	for (auto col : *m_columnDefinitionList) {
		desiredSize.Width += col->GetAvailableDesiredWidth();
	}
	for (auto row : *m_rowDefinitionList) {
		desiredSize.Height += row->GetAvailableDesiredHeight();
	}

	// this の m_desiredSize を設定する
	Panel::MeasureLayout(desiredSize);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Grid::ArrangeLayout(const RectF& finalLocalRect)
{
	// "Auto" と "Pixel" 指定である Column/Row の最終サイズを確定させる
	SizeF totalActual = SizeF::Zero;
	int starColCount = 0;
	int starRowCount = 0;
	for (auto col : *m_columnDefinitionList)
	{
		if (col->IsAuto() || col->IsPixel())
		{
			col->m_actualWidth = col->GetAvailableDesiredWidth();
			totalActual.Width += col->m_actualWidth;
		}
		else {
			starColCount++;
		}
	}
	for (auto row : *m_rowDefinitionList)
	{
		if (row->IsAuto() || row->IsPixel())
		{
			row->m_actualHeight = row->GetAvailableDesiredHeight();
			totalActual.Height += row->m_actualHeight;
		}
		else {
			starRowCount++;
		}
	}

	// "*" ひとつ分のセルの領域
	SizeF starUnit(
		(finalLocalRect.Width - totalActual.Width) / starColCount,
		(finalLocalRect.Height - totalActual.Height) / starRowCount);
	starUnit.Width = std::max(0.0f, starUnit.Width);	// 負値はダメ
	starUnit.Height = std::max(0.0f, starUnit.Height);	// 負値はダメ

	// "*" 指定である Column/Row の最終サイズを確定させ、
	// 全セルのオフセット (位置) も確定させる
	PointF totalOffset = PointF::Zero;
	for (auto col : *m_columnDefinitionList)
	{
		if (col->IsStar()) {
			col->m_actualWidth = starUnit.Width;
		}

		// セルX座標確定
		col->m_actualOffsetX = totalOffset.X;
		totalOffset.X += col->m_actualWidth;
	}
	for (auto row : *m_rowDefinitionList)
	{
		if (row->IsStar()) {
			row->m_actualHeight = starUnit.Height;
		}

		// セルY座標確定
		row->m_actualOffsetY = totalOffset.Y;
		totalOffset.Y += row->m_actualHeight;
	}

	// 子要素の最終位置・サイズを確定させる
	for (auto child : *m_children)
	{
		int colIdx = Grid::GetColumn(child);
		int rowIdx = Grid::GetRow(child);
		int colSpan = Grid::GetColumnSpan(child);
		int rowSpan = Grid::GetRowSpan(child);
		colSpan = std::max(1, colSpan);	// 0 は 1 とする
		rowSpan = std::max(1, rowSpan);	// 0 は 1 とする
		colSpan = std::min(colSpan, colIdx + m_columnDefinitionList->GetCount());	// 最大値制限
		rowSpan = std::min(rowSpan, rowIdx + m_rowDefinitionList->GetCount());		// 最大値制限

		// 配置先のセルで座標を確定
		RectF rect(
			m_columnDefinitionList->GetAt(colIdx)->m_actualOffsetX,
			m_rowDefinitionList->GetAt(rowIdx)->m_actualOffsetY,
			0, 0);

		// Span を考慮してサイズを確定
		for (int iCol = 0; iCol < colSpan; ++iCol) {
			rect.Width += m_columnDefinitionList->GetAt(colIdx + iCol)->m_actualWidth;
		}
		for (int iRow = 0; iRow < rowSpan; ++iRow) {
			rect.Height += m_rowDefinitionList->GetAt(rowIdx + iRow)->m_actualHeight;
		}

		// Arrange
		child->ArrangeLayout(rect);
	}

	Panel::ArrangeLayout(finalLocalRect);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void	Grid::SetColumn(UIElement* element, int value)		{ LN_SET_ATTACHED_PROPERTY(element, ColumnProperty, value); }
int		Grid::GetColumn(UIElement* element)					{ LN_GET_ATTACHED_PROPERTY(element, ColumnProperty, int); }
void	Grid::SetColumnSpan(UIElement* element, int value)	{ LN_SET_ATTACHED_PROPERTY(element, ColumnSpanProperty, value); }
int		Grid::GetColumnSpan(UIElement* element)				{ LN_GET_ATTACHED_PROPERTY(element, ColumnSpanProperty, int); }
void	Grid::SetRow(UIElement* element, int value)			{ LN_SET_ATTACHED_PROPERTY(element, RowProperty, value); }
int		Grid::GetRow(UIElement* element)					{ LN_GET_ATTACHED_PROPERTY(element, RowProperty, int); }
void	Grid::SetRowSpan(UIElement* element, int value)		{ LN_SET_ATTACHED_PROPERTY(element, RowSpanProperty, value); }
int		Grid::GetRowSpan(UIElement* element)				{ LN_GET_ATTACHED_PROPERTY(element, RowSpanProperty, int); }


} // namespace GUI
} // namespace Lumino

