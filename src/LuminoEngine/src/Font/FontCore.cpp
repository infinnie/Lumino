﻿
#include "Internal.hpp"
#include "FontManager.hpp"
#include "FontCore.hpp"

namespace ln {
namespace detail {

//==============================================================================
// FontCore
	
FontCore::FontCore()
	: m_manager(nullptr)
{
}

void FontCore::initialize(FontManager* manager)
{
	if (LN_REQUIRE(manager)) return;
	m_manager = manager;
	m_manager->addAliveFontCore(this);
}

void FontCore::dispose()
{
	if (m_manager)
	{
		m_manager->removeAliveFontCore(this);
		m_manager = nullptr;
	}
}

void FontCore::finalize()
{
	// ObjectCache から削除されるとき用
	dispose();
}

} // namespace detail
} // namespace ln

