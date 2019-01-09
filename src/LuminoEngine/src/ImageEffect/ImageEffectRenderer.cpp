﻿
#include "Internal.hpp"
#include <LuminoEngine/Rendering/RenderingContext.hpp>
#include <LuminoEngine/Rendering/Material.hpp>
#include <LuminoEngine/ImageEffect/ImageEffect.hpp>
#include "ImageEffectRenderer.hpp"
#include "../Rendering/RenderTargetTextureCache.hpp"
#include "../Rendering/RenderingManager.hpp"

namespace ln {
namespace detail {

//==============================================================================
// ImageEffectRenderer

ImageEffectRenderer::ImageEffectRenderer()
    : m_manager(detail::EngineDomain::renderingManager())
{
}

void ImageEffectRenderer::addImageEffect(ImageEffect* effect)
{
    if (LN_REQUIRE(effect)) return;
    m_imageEffects.add(effect);
}

void ImageEffectRenderer::removeImageEffect(ImageEffect* effect)
{
    if (LN_REQUIRE(effect)) return;
    m_imageEffects.remove(effect);
}

void ImageEffectRenderer::updateFrame(float elapsedSeconds)
{
    for (auto& imageEffect : m_imageEffects) {
        imageEffect->updateFrame(elapsedSeconds);
    }
}

void ImageEffectRenderer::render(RenderingContext* context, RenderTargetTexture* inout)
{
    if (!m_imageEffects.isEmpty())
    {
        RenderTargetTexture* primaryTarget = m_manager->frameBufferCache()->requestRenderTargetTexture(
            SizeI(inout->width(), inout->height()), TextureFormat::RGBA32, false);
        RenderTargetTexture* secondaryTarget = m_manager->frameBufferCache()->requestRenderTargetTexture(
            SizeI(inout->width(), inout->height()), TextureFormat::RGBA32, false);

        context->pushState(true);
        context->setDepthBuffer(nullptr);

        for (int i = 0; i < m_imageEffects.size(); i++)
        {
            if (i == 0) {
                m_imageEffects[i]->onRender(context, inout, secondaryTarget);
            }
            else {
                m_imageEffects[i]->onRender(context, primaryTarget, secondaryTarget);
            }
            std::swap(primaryTarget, secondaryTarget);
        }

        context->resetState();

        context->blit(primaryTarget, inout);

        context->popState();

        m_manager->frameBufferCache()->release(secondaryTarget);
        m_manager->frameBufferCache()->release(primaryTarget);
    }
}

} // namespace detail
} // namespace ln

