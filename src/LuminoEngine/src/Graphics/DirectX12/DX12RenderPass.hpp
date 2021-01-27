﻿
#pragma once
#include "DX12Helper.hpp"
#include "DX12Texture.hpp"

namespace ln {
namespace detail {
	
class DX12RenderPass
	: public IRenderPass
{
public:
    DX12RenderPass();
	bool init(DX12Device* device, const DeviceFramebufferState& buffers, ClearFlags clearFlags, const Color& clearColor, float clearDepth, uint8_t clearStencil);
	void dispose() override;

	DX12RenderTarget* renderTarget(int index) const { return static_cast<DX12RenderTarget*>(m_renderTargets[index].get()); }
	DX12DepthBuffer* depthBuffer() const { return static_cast<DX12DepthBuffer*>(m_depthBuffer.get()); }

	ClearFlags clearFlags() const { return m_clearFlags; }
	const Color& clearColor() const { return m_clearColor; }
	float clearDepth() const { return m_clearDepth; }
	uint8_t clearStencil() const { return m_clearStencil; }

private:
	DX12Device* m_device;
	ClearFlags m_clearFlags;
	Color m_clearColor;
	float m_clearDepth;
	uint8_t m_clearStencil;
};

} // namespace detail
} // namespace ln
