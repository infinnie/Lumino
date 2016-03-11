
#pragma once
#include "Texture.h"

LN_NAMESPACE_BEGIN

class RenderingCompositionContext
{
public:
	RenderTarget* RequestRenderTarget(int width, int height);
};

// Camera とかが継承する
class IRenderingCompositionItem
{
public:
	void Render(RenderingCompositionContext* context);
};

/**
	@brief		
*/
class Viewport
	: public Object
{
public:


LN_INTERNAL_ACCESS:	// TODO: いまはとりあえず内部用途
	Viewport();
	virtual ~Viewport();
	void Initialize(RenderTarget* renderTarget);

private:
	RenderTarget*	m_renderTarget;
};

LN_NAMESPACE_END
