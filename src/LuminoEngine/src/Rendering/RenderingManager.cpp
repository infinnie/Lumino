﻿
#include "Internal.hpp"
#include <Lumino/Graphics/VertexDeclaration.hpp>
#include "../Engine/LinearAllocator.hpp"
#include "DrawElementListBuilder.hpp"
#include "SpriteRenderFeature.hpp"
#include "RenderingManager.hpp"

namespace ln {
namespace detail {

//==============================================================================
// RenderingManager
	
RenderingManager::RenderingManager()
	: m_graphicsManager(nullptr)
	, m_standardVertexDeclaration()
	, m_spriteRenderFeature()
	, m_stageDataPageManager()
{
}

void RenderingManager::initialize(const Settings& settings)
{
	m_graphicsManager = settings.graphicsManager;

	static VertexElement elements[] =
	{
		{ 0, VertexElementType::Float3, VertexElementUsage::Position, 0 },
		{ 0, VertexElementType::Float3, VertexElementUsage::Normal, 0 },
		{ 0, VertexElementType::Float2, VertexElementUsage::TexCoord, 0 },
		{ 0, VertexElementType::Float4, VertexElementUsage::Color, 0 },
	};
	m_standardVertexDeclaration = newObject<VertexDeclaration>(elements, 4);

	m_renderStageListBuilder = makeRef<DrawElementListBuilder>();

	m_spriteRenderFeature = newObject<SpriteRenderFeature>(this);

	m_stageDataPageManager = makeRef<LinearAllocatorPageManager>();
}

void RenderingManager::dispose()
{
	m_stageDataPageManager = nullptr;
	m_spriteRenderFeature = nullptr;
	m_renderStageListBuilder = nullptr;
	m_standardVertexDeclaration = nullptr;
}

} // namespace detail
} // namespace ln

