﻿
#include "Internal.hpp"
#include <LuminoEngine/Graphics/Texture.hpp>
#include <LuminoEngine/Rendering/Material.hpp>
#include <LuminoEngine/Rendering/RenderingContext.hpp>
#include <LuminoEngine/ImageEffect/ToneImageEffect.hpp>

namespace ln {

//==============================================================================
// ToneImageEffect

Ref<ToneImageEffect> ToneImageEffect::create()
{
    return makeObject<ToneImageEffect>();
}

ToneImageEffect::ToneImageEffect()
{
}

ToneImageEffect::~ToneImageEffect()
{
}

void ToneImageEffect::init()
{
    ImageEffect::init();
    auto shader = makeObject<Shader>(u"D:/Proj/Volkoff/Engine/Lumino/src/LuminoEngine/src/ImageEffect/Resource/ToneImageEffect.fx");

    m_material = makeObject<Material>();
    m_material->setShader(shader);
}

void ToneImageEffect::play(const ColorTone& tone, double time)
{
    m_toneValue.start(tone.toVector4(), time);
}

void ToneImageEffect::onUpdateFrame(float elapsedSeconds)
{
    m_toneValue.advanceTime(elapsedSeconds);
}

void ToneImageEffect::onRender(RenderingContext* context, RenderTargetTexture* source, RenderTargetTexture* destination)
{
    m_material->setVector(u"_Tone", m_toneValue.getValue());
    context->blit(source, destination, m_material);
}

} // namespace ln
