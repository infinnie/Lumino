﻿
#ifdef LN_GLFW

#include "Internal.hpp"
#include "GLFWSwapChain.hpp"
#include "../../Platform/GLFWPlatformWindowManager.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ln {
namespace detail {

//=============================================================================
// GLFWSwapChain

GLFWSwapChain::GLFWSwapChain(OpenGLDevice* device)
	: GLSwapChain(device) 
	, m_window(nullptr)
{
}

bool GLFWSwapChain::init(PlatformWindow* window, const SizeI& backbufferSize)
{
	m_window = dynamic_cast<GLFWPlatformWindow*>(window);
	if (LN_REQUIRE(m_window)) return false;
	genBackbuffer(backbufferSize.width, backbufferSize.height);
	return true;
}

void GLFWSwapChain::getBackendBufferSize(SizeI* outSize)
{
	m_window->getFramebufferSize(&outSize->width, &outSize->height);
}

void GLFWSwapChain::beginMakeContext()
{
	if (m_window == device()->mainWindow()) {
	}
	else {
		glfwMakeContextCurrent(window()->glfwWindow());
	}
}

void GLFWSwapChain::endMakeContext()
{
	if (m_window == device()->mainWindow()) {
	}
	else {
		// MainWindow に戻す
		const auto glfwWindow = static_cast<GLFWPlatformWindow*>(device()->mainWindow());
		glfwMakeContextCurrent(glfwWindow->glfwWindow());
	}
}

void GLFWSwapChain::swap()
{
	glfwSwapBuffers(window()->glfwWindow());
}

////=============================================================================
//// GLFWContext
//
//void GLFWContext::init(OpenGLDevice* device, PlatformWindow* window)
//{
//	m_device = device;
//	auto* glfwWindow = dynamic_cast<GLFWPlatformWindow*>(window);
//	LN_CHECK(glfwWindow);
//	m_mainWindow = glfwWindow;
//
//	glfwMakeContextCurrent(m_mainWindow->glfwWindow());
//}
//
//Ref<GLSwapChain> GLFWContext::createSwapChain(PlatformWindow* window, const SizeI& backbufferSize)
//{
//	auto* glfwWindow = dynamic_cast<GLFWPlatformWindow*>(window);
//	LN_CHECK(glfwWindow);
//
//	auto ptr = makeRef<GLFWSwapChain>(m_device, glfwWindow);
//	ptr->genBackbuffer(backbufferSize.width, backbufferSize.height);
//
//	return ptr;
//}


} // namespace detail
} // namespace ln

#endif