﻿
#include "Internal.hpp"
#include "VisualManager.hpp"
#include <LuminoEngine/Visual/SpriteComponent.hpp>
#include <LuminoEngine/Engine/Property.hpp>	// TODO:
#include "../Engine/EngineManager.hpp"	// TODO:

namespace ln {
namespace detail {

//=============================================================================
// VisualManager

VisualManager::VisualManager()
	: m_graphicsManager(nullptr)
{
}

VisualManager::~VisualManager()
{
}

void VisualManager::init(const Settings& settings)
{
	m_graphicsManager = settings.graphicsManager;

	SpriteComponent::registerType(detail::EngineDomain::engineManager()->engineContext());
}

void VisualManager::dispose()
{
}

} // namespace detail
} // namespace ln

