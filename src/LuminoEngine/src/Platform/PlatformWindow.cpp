﻿
#include "Internal.hpp"
#include <Lumino/Platform/PlatformWindow.hpp>

namespace ln {
namespace detail {

//==============================================================================
// PlatformWindow

PlatformWindow::PlatformWindow()
	: m_eventListeners()
{
}

void PlatformWindow::attachEventListener(IPlatforEventListener* listener)
{
	m_eventListeners.add(listener);
}

void PlatformWindow::detachEventListener(IPlatforEventListener* listener)
{
	m_eventListeners.remove(listener);
}

bool PlatformWindow::sendEventToAllListener(const PlatformEventArgs& e)
{
	for (IPlatforEventListener* listener : m_eventListeners) {
		if (listener->onPlatformEvent(e)) {
			return true;
		}
	}
	return false;
}

} // namespace detail
} // namespace ln

