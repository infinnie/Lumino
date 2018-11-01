﻿
#pragma once
#include "Common.hpp"
#include "ColorStructs.hpp"

namespace ln {

namespace detail {

struct ClColor
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct U32
{
	byte_t D[4];
};

class PixelAccessor_R8G8B8A8
{
public:
	inline size_t pitch(int width) const
	{
		return width * 4;
	}
	inline ClColor get(const byte_t* line, int x) const
	{
		U32* c = &((U32*)line)[x];
		return ClColor{ c->D[0], c->D[1], c->D[2], c->D[3] };
	}
	inline void set(byte_t* line, int x, ClColor color) const
	{
		U32* w = &((U32*)line)[x];
		w->D[0] = color.r; w->D[1] = color.g; w->D[2] = color.b; w->D[3] = color.a;
	}
};

class PixelAccessor_A8
{
public:
	ClColor color;

	inline size_t pitch(int width) const
	{
		return width;
	}
	inline ClColor get(const byte_t* line, int x) const
	{
		byte_t c = line[x];
		return ClColor{ color.r, color.g, color.b, c };
	}
	inline void set(byte_t* line, int x, ClColor color) const
	{
		line[x] = color.a;
	}
};

template<class TPixelAccessor>
class SrcBuffer
{
public:
	/// bitmap	: 転送元 RawBitmap
	/// srcRect	: 転送元領域 (RawBitmap のサイズに収まるようにクリッピングされていること)
	SrcBuffer(const byte_t* data, size_t width, bool upFlow, const RectI& srcRect, TPixelAccessor accessor)
		: m_data(data)
		, m_widthPitch(accessor.pitch(width))
		, m_srcRect(srcRect)
		, m_bottomLine(srcRect.getBottom() - 1)	// 転送範囲の最後の行 (0スタート)
		, m_currentLine(nullptr)
		, m_upFlow(upFlow)
		, m_accessor(accessor)
	{}

public:
	inline void setLine(int y)
	{
		if (!m_upFlow)
			m_currentLine = &m_data[(m_widthPitch * (m_srcRect.y + y))];
		else
			m_currentLine = &m_data[(m_widthPitch * (m_bottomLine - y))];
	}

	inline ClColor getPixel(int x) const
	{
		return m_accessor.get(m_currentLine, m_srcRect.x + x);
	}

private:
	const byte_t* m_data;
	size_t m_widthPitch;
	const RectI&	m_srcRect;
	int				m_bottomLine;
	const byte_t*	m_currentLine;
	bool			m_upFlow;
	TPixelAccessor m_accessor;
};

template<class TPixelAccessor>
class DestBuffer
{
public:
	/// bitmap	: 転送先 RawBitmap
	/// rect	: 転送先領域 (RawBitmap のサイズに収まるようにクリッピングされていること)
	DestBuffer(byte_t* data, size_t width, bool upFlow, const RectI& dstRect, TPixelAccessor accessor)
		: m_data(data)
		, m_widthPitch(accessor.pitch(width))
		, m_dstRect(dstRect)
		, m_bottomLine(dstRect.getBottom() - 1)	// 転送範囲の最後の行 (0スタート)
		, m_currentLine(nullptr)
		, m_upFlow(upFlow)
		, m_accessor(accessor)
	{}

public:
	inline void setLine(int y)
	{
		if (!m_upFlow)
			m_currentLine = &m_data[(m_widthPitch * (m_dstRect.y + y))];
		else
			m_currentLine = &m_data[(m_widthPitch * (m_bottomLine - y))];
	}

	inline ClColor getPixel(int x)
	{
		return m_accessor.get(m_curLine, m_dstRect.x + x);
	}

	inline void setPixel(int x, ClColor color)
	{
		m_accessor.set(m_currentLine, m_dstRect.x + x, color);
	}

private:
	byte_t*			m_data;
	int				m_widthPitch;
	const RectI&	m_dstRect;
	int				m_bottomLine;
	byte_t*			m_currentLine;
	bool			m_upFlow;
	TPixelAccessor m_accessor;
};

} // namespace detail 

class Bitmap2D
	: public Object
{
public:
	int width() const { return m_size.width; }
	int height() const { return m_size.height; }
	PixelFormat format() const { return m_format; }

	Color32 getPixel32(int x, int y) const;
	void setPixel32(int x, int y, const Color32& color);

	/** バッファの先頭へのポインタを取得します。 */
	byte_t* data() LN_NOEXCEPT { return m_buffer->data(); }

	/** バッファの先頭へのポインタを取得します。 */
	const byte_t* data() const LN_NOEXCEPT { return m_buffer->data(); }

	void flipVerticalFlow();

	void load(const StringRef& filePath);
	void save(const StringRef& filePath);

	Ref<Bitmap2D> transcodeTo(PixelFormat format, const Color32& color = Color32::White) const;

	static int getPixelFormatByteSize(PixelFormat format);
	static int getBitmapByteSize(int width, int height, int depth, PixelFormat format);

LN_CONSTRUCT_ACCESS:
	Bitmap2D();
	virtual ~Bitmap2D();
	void initialize();
	void initialize(int width, int height, PixelFormat format);

LN_INTERNAL_ACCESS:
	ByteBuffer* rawBuffer() { return m_buffer; }

public:	// TODO:
	//Ref<Bitmap2D> transcodeA8ToRGBA32() const;

	Ref<ByteBuffer> m_buffer;	// Font などからの reserve 動作のため、m_size * m_format とバイトサイズは一致しないことがあるので注意。
	SizeI m_size;
	PixelFormat m_format;
};

class Bitmap3D
	: public Object
{
public:
	int width() const { return m_width; }
	int height() const { return m_height; }
	int depth() const { return m_height; }
	PixelFormat format() const { return m_format; }

	void setPixel32(int x, int y, int z, const Color32& color);

	/** バッファの先頭へのポインタを取得します。 */
	byte_t* data() LN_NOEXCEPT { return m_buffer->data(); }

	/** バッファの先頭へのポインタを取得します。 */
	const byte_t* data() const LN_NOEXCEPT { return m_buffer->data(); }

LN_CONSTRUCT_ACCESS:
	Bitmap3D();
	virtual ~Bitmap3D();
	void initialize(int width, int height, int depth, PixelFormat format);

LN_INTERNAL_ACCESS:
	ByteBuffer* rawBuffer() { return m_buffer; }

private:
	Ref<ByteBuffer> m_buffer;
	int m_width;
	int m_height;
	int m_depth;
	PixelFormat m_format;
};

namespace detail {

class BitmapHelper
{
public:
	static void blitRawSimple(void* dst, const void* src, size_t width, size_t height, size_t pixelBytes, bool flipVertical);
	static void blitRawSimple3D(void* dst, const void* src, size_t width, size_t height, size_t depth, size_t pixelBytes, bool flipVertical);
};
} // namespace detail
} // namespace ln
