﻿#pragma once

#include "Common.hpp"
#include "GeometryStructs.hpp"
#include "ColorStructs.hpp"
#include "RenderState.hpp"

namespace ln {
class VertexDeclaration;
class VertexBuffer;
class IndexBuffer;
class RenderTargetTexture;
class DepthBuffer;
class Shader;
class ShaderPass;
class SwapChain;

namespace detail {
	class GraphicsManager;
	class IGraphicsDeviceContext;
}

class LN_API GraphicsContext
	: public Object
{
public:
	void resetState();
	void setBlendState(const BlendStateDesc& value);
	void setRasterizerState(const RasterizerStateDesc& value);
	void setDepthStencilState(const DepthStencilStateDesc& value);
	void setColorBuffer(int index, RenderTargetTexture* value);	// Viewport, Scissor はクリア TODO: やっぱり ColorBuffer は驚く
    RenderTargetTexture* colorBuffer(int index) const;
	void setDepthBuffer(DepthBuffer* value);
    DepthBuffer* depthBuffer() const;
	void setViewportRect(const Rect& value);
	void setScissorRect(const Rect& value);
	void setVertexDeclaration(VertexDeclaration* value);
	void setVertexBuffer(int streamIndex, VertexBuffer* value);
	void setIndexBuffer(IndexBuffer* value);
	void setShaderPass(ShaderPass* value);

	void clear(ClearFlags flags, const Color& color, float z = 1.0f, uint8_t stencil = 0x00);
	void drawPrimitive(PrimitiveType primitive, int startVertex, int primitiveCount);
	void drawPrimitiveIndexed(PrimitiveType primitive, int startIndex, int primitiveCount);
	void present(SwapChain* swapChain);

	// TODO: internal
	// IGraphicsDeviceContext の clear, draw 系の機能を呼び出したい場合はこの戻り値を使うこと。
	// GraphicsContext は変更中のステートをキャッシュするが、それを確実に IGraphicsDeviceContext へ送信した状態にする。
	detail::IGraphicsDeviceContext* commitState();

LN_CONSTRUCT_ACCESS:
	GraphicsContext();
	virtual ~GraphicsContext();
	void init(detail::IGraphicsDeviceContext* device);

LN_INTERNAL_ACCESS:
	virtual void dispose() override;

private:

	detail::GraphicsManager* m_manager;
	detail::IGraphicsDeviceContext* m_device;

    // TODO: 途中
    enum ModifiedFlags
    {
        ModifiedFlags_None = 0,
        ModifiedFlags_VertexBuffers = 1 << 2,
        ModifiedFlags_IndexBuffer = 1 << 3,
        ModifiedFlags_ShaderPass = 1 << 4,
        ModifiedFlags_All = 0xFFFFFFFF,
    };

	struct State
	{
		BlendStateDesc blendState;
		RasterizerStateDesc rasterizerState;
		DepthStencilStateDesc depthStencilState;
		std::array<Ref<RenderTargetTexture>, 4> renderTargets;
		Ref<DepthBuffer> depthBuffer;
		Rect viewportRect;
		Rect scissorRect;
		Ref<VertexDeclaration> vertexDeclaration;
		std::array<Ref<VertexBuffer>, 4> vertexBuffers;
		Ref<IndexBuffer> indexBuffer;
		Ref<Shader> shader;		// shaderPass owner, for keep reference.
		ShaderPass* shaderPass;

		void reset();
	};

	State m_staging;
	State m_lastCommit;
    uint32_t m_modifiedFlags;
};

} // namespace ln
