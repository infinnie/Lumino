﻿
#include "Internal.hpp"
#include "EngineManager.hpp"
//#include "EngineDomain.hpp"
#include <Lumino/Engine/Engine.hpp>
#include <Lumino/Engine/EngineSettings.hpp>
#include <Lumino/Graphics/GraphicsContext.hpp>
#include <Lumino/UI/UIFrameWindow.hpp>
#include "../Graphics/GraphicsManager.hpp"

namespace ln {

//==============================================================================
// Engine

/** メインウィンドウのクライアント領域の幅と高さを設定します。(default: 640x480) */
void EngineSettings::setMainWindowSize(int width, int height)
{
	detail::EngineDomain::engineManager()->settings().mainWindowSize.set(width, height);
}

void EngineSettings::setMainBackBufferSize(int width, int height)
{
	detail::EngineDomain::engineManager()->settings().mainBackBufferSize.set(width, height);
}

void EngineSettings::setMainWindowTitle(const StringRef& title)
{
	detail::EngineDomain::engineManager()->settings().mainWindowTitle = title;
}

//==============================================================================
// Engine

static void beginFrame()
{
	detail::EngineDomain::engineManager()->updateFrame();
	detail::EngineDomain::engineManager()->renderFrame();
}

static void endFrame()
{
	detail::EngineDomain::engineManager()->presentFrame();
}

void Engine::initialize()
{
	detail::EngineDomain::engineManager()->initialize();
	beginFrame();
}

void Engine::terminate()
{
	endFrame();
	detail::EngineDomain::release();
}

bool Engine::update()
{
	endFrame();
	beginFrame();
	return !detail::EngineDomain::engineManager()->isExitRequested();
}

GraphicsContext* Engine::graphicsContext()
{
	return detail::EngineDomain::graphicsManager()->graphicsContext();
}

UIFrameWindow* Engine::mainWindow()
{
	return detail::EngineDomain::engineManager()->mainWindow();
}

} // namespace ln

