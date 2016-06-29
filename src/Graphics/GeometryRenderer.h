﻿
#pragma once
#include <Lumino/Graphics/Common.h>
#include <Lumino/Base/Rect.h>
#include <Lumino/Math/Matrix.h>
#include <Lumino/Graphics/Brush.h>

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GRAPHICS_BEGIN
namespace detail
{
class DrawingContextImpl;

enum class DrawingClass : uint32_t
{
	Unknown,
	PathStroke,
	PointList,
	LineList,
	TriangleList,
	GryphRun,		// TODO: いらないかも。専用クラスに任せたい
};


struct DrawingState
{
	Matrix			transform;
	RefPtr<Brush>	brush;
	RefPtr<Pen>		pen;
	RefPtr<Font>	font;
	float			opacity = 1.0f;
	ToneF			tone;
};



class GeometryRenderer
	: public RefObject
	, public detail::IRendererPloxy
{
public:

	void SetViewProjection(const Matrix& view, const Matrix& proj, const Size& viewPixelSize);

	void SetTransform(const Matrix& matrix);
	void SetBrush(Brush* brush);
	void SetPen(Pen* pen);
	void SetOpacity(float opacity);	// 0~1
	void SetTone(const ToneF& tone);
	void SetFont(Font* font);

	void MoveTo(const Vector3& point, const ColorF& color);
	void LineTo(const Vector3& point, const ColorF& color);
	void BezierCurveTo(const Vector3& cp1, const Vector3& cp2, const Vector3& endPt, const ColorF& color);
	void ClosePath();

	//void DrawPoint(const Vector3& point, const ColorF& color);

	/**
		@brief		直線を描画します。
	*/
	void DrawLine(const Vector3& from, const Vector3& to, const ColorF& fromColor, const ColorF& toColor);
	void DrawLine(const Vector3& from, const Vector3& to, const ColorF& color);

	void DrawTriangle(const Vector3& p1, const ColorF& p1Color, const Vector3& p2, const ColorF& p2Color, const Vector3& p3, const ColorF& p3Color);

	/**
		@brief		矩形を描画します。
	*/
	void DrawRectangle(const RectF& rect, const ColorF& color);

	void DrawEllipse(const Vector3& center, const Vector2& radius);

	void DrawTexture(const RectF& rect, Texture* texture, const Rect& secRect, const ColorF& color);

	//void Flush();
	virtual void Flush() override;
	virtual void OnActivated() override {}
	virtual void OnDeactivated() override { Flush(); }

LN_INTERNAL_ACCESS:
	GeometryRenderer();
	virtual ~GeometryRenderer();
	void Initialize(GraphicsManager* manager);

private:
	void AddCommand(const void* command, size_t size);
	void FlushInternal();
	void SetDrawingClassInternal(detail::DrawingClass dc);
	//void CheckFlush();

	GraphicsManager*		m_manager;
	DrawingContextImpl*		m_internal;
	ByteBuffer				m_commandsBuffer;
	size_t					m_commandsUsingByte;
	detail::DrawingClass	m_currentDrawingClass;
	detail::DrawingState	m_currentState;
	
	Vector2					m_uvParPixel;
	Matrix					m_invViewProj;
	Vector2					m_viewOnePixelOffset;

	bool					m_flushRequested;
	//bool					m_stateModified;

	TextureBrush*			m_internalTextureBrush;
};

} // namespace detail

LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END
