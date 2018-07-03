﻿#pragma once

namespace ln {
struct SizeI;
struct Color;

namespace detail {
class PlatformWindow;
class IShaderUniform;

class IVertexDeclaration
	: public RefObject
{
protected:
	virtual ~IVertexDeclaration() = default;
};

class IVertexBuffer
	: public RefObject
{
public:
	virtual void setSubData(size_t offset, const void* data, size_t length) = 0;
	virtual void* map(size_t offset, uint32_t length) = 0;
	virtual void unmap() = 0;

protected:
	virtual ~IVertexBuffer() = default;
};

class IIndexBuffer
	: public RefObject
{
public:
	virtual void setSubData(size_t offset, const void* data, size_t length) = 0;
	virtual void* map(size_t offset, uint32_t length) = 0;
	virtual void unmap() = 0;

protected:
	virtual ~IIndexBuffer() = default;
};

class ITexture
	: public RefObject
{
public:


	virtual void getSize(SizeI* outSize) = 0;
	virtual TextureFormat getTextureFormat() const = 0;

	// データは up flow (上下反転)
	virtual void readData(void* outData) = 0;


	//void Clear(const Color& color);

	//void setSubData(const PointI& point, RawBitmap* bitmap);



	/// テクスチャの種類の取得
	//virtual TextureType getTextureType() const = 0;

	/// サーフェイスフォーマットの取得
	//virtual TextureFormat getTextureFormat() const = 0;


	///// 実際のサイズの取得 (デバイス依存により2のべき乗倍に拡張されたサイズ)
	//virtual const SizeI& getRealSize() const = 0;

	///// サンプラステートの設定
	////virtual void setSamplerState(const SamplerState& state) = 0;

	///// データ転送 (TODO:部分更新は未実装…)
	//// data に渡されるイメージデータは上下が反転している状態。
	//virtual void setSubData(const PointI& point, const void* data, size_t dataBytes, const SizeI& dataBitmapSize) = 0;

	//virtual void setSubData3D(const Box32& box, const void* data, size_t dataBytes) = 0;

	//// (得られるデータは上下反転)
	//virtual void getData(const RectI& rect, void* outData) = 0;

	// 得られるデータは上下反転していない。レイアウトは RGBA
	//virtual void readData(void* outData) {}


	///// ロック (バッファは上下反転)
	//virtual RawBitmap* lock() = 0;

	///// アンロック
	//virtual void unlock() = 0;

protected:
	virtual ~ITexture() = default;
};

class IDepthBuffer
	: public RefObject
{
public:

protected:
	virtual ~IDepthBuffer() = default;
};

class ISwapChain
	: public RefObject
{
public:
	virtual ITexture* colorBuffer() const = 0;

protected:
	virtual ~ISwapChain() = default;
};

class IShaderPass
	: public RefObject
{
public:
	virtual int getUniformCount() const = 0;
	virtual IShaderUniform* getUniform(int index) const = 0;
	virtual void setUniformValue(int index, const void* data, size_t size) = 0;

protected:
	virtual ~IShaderPass() = default;
};

class IShaderUniform
	: public RefObject
{
public:
	virtual const ShaderUniformTypeDesc& desc() const = 0;
	virtual const std::string& name() const = 0;

protected:
	virtual ~IShaderUniform() = default;
};

class IGraphicsDeviceContext
	: public RefObject
{
public:
	IGraphicsDeviceContext();
	virtual ~IGraphicsDeviceContext() = default;

	void initialize();
	virtual void dispose();
	void enterMainThread();
	void leaveMainThread();
	//void enterRenderingThread();
	//void leaveRenderingThread();

	Ref<ISwapChain> createSwapChain(PlatformWindow* window, const SizeI& backbufferSize);
	Ref<IVertexDeclaration> createVertexDeclaration(const VertexElement* elements, int elementsCount);
	Ref<IVertexBuffer> createVertexBuffer(GraphicsResourceUsage usage, size_t bufferSize, const void* initialData = nullptr);
	Ref<IIndexBuffer> createIndexBuffer(GraphicsResourceUsage usage, IndexBufferFormat format, int indexCount, const void* initialData = nullptr);
	Ref<ITexture> createRenderTarget(uint32_t width, uint32_t height, TextureFormat requestFormat, bool mipmap);
	Ref<IShaderPass> createShaderPass(const byte_t* vsCode, int vsCodeLen, const byte_t* fsCodeLen, int psCodeLen, ShaderCompilationDiag* diag);


	void setColorBuffer(int index, ITexture* value);
	void setDepthBuffer(IDepthBuffer* value);
	void setVertexDeclaration(IVertexDeclaration* value);
	void setVertexBuffer(int streamIndex, IVertexBuffer* value);
	void setIndexBuffer(IIndexBuffer* value);
	void setShaderPass(IShaderPass* value);

	void clearBuffers(ClearFlags flags, const Color& color, float z, uint8_t stencil);
	void drawPrimitive(PrimitiveType primitive, int startVertex, int primitiveCount);
	void drawPrimitiveIndexed(PrimitiveType primitive, int startIndex, int primitiveCount);

	void present(ISwapChain* swapChain);

protected:
	virtual void onEnterMainThread() = 0;
	virtual void onLeaveMainThread() = 0;
	virtual void onEnterRenderState() = 0;
	virtual void onLeaveRenderState() = 0;
	virtual Ref<ISwapChain> onCreateSwapChain(PlatformWindow* window, const SizeI& backbufferSize) = 0;
	virtual Ref<IVertexDeclaration> onCreateVertexDeclaration(const VertexElement* elements, int elementsCount) = 0;
	virtual Ref<IVertexBuffer> onCreateVertexBuffer(GraphicsResourceUsage usage, size_t bufferSize, const void* initialData) = 0;
	virtual Ref<IIndexBuffer> onCreateIndexBuffer(GraphicsResourceUsage usage, IndexBufferFormat format, int indexCount, const void* initialData) = 0;
	virtual Ref<ITexture> onCreateRenderTarget(uint32_t width, uint32_t height, TextureFormat requestFormat, bool mipmap) = 0;
	virtual Ref<IShaderPass> onCreateShaderPass(const byte_t* vsCode, int vsCodeLen, const byte_t* fsCodeLen, int psCodeLen, ShaderCompilationDiag* diag) = 0;

	virtual void onUpdateFrameBuffers(ITexture** renderTargets, int renderTargetsCount, IDepthBuffer* depthBuffer) = 0;
	virtual void onUpdatePrimitiveData(IVertexDeclaration* decls, IVertexBuffer** vertexBuufers, int vertexBuffersCount, IIndexBuffer* indexBuffer) = 0;

	virtual void onClearBuffers(ClearFlags flags, const Color& color, float z, uint8_t stencil) = 0;
	virtual void onDrawPrimitive(PrimitiveType primitive, int startVertex, int primitiveCount) = 0;
	virtual void onDrawPrimitiveIndexed(PrimitiveType primitive, int startIndex, int primitiveCount) = 0;

	//virtual void onSetShaderPass(IShaderPass* pass) = 0;

	virtual void onPresent(ISwapChain* swapChain) = 0;

private:
	void commitStatus();

	struct State
	{
		std::array<ITexture*, 4> renderTargets = {};
		IDepthBuffer* depthBuffer = nullptr;
		IVertexDeclaration* vertexDeclaration = nullptr;
		std::array<IVertexBuffer*, 4> vertexBuffers = {};
		IIndexBuffer* indexBuffer = nullptr;
		IShaderPass* shaderPass = nullptr;
	};

	State m_staging;
};


} // namespace detail
} // namespace ln

