﻿
#include "Internal.hpp"
#include <LuminoEngine/Visual/LightComponent.hpp>
#include <LuminoEngine/Scene/WorldObject.hpp>
//#include "../Rendering/ClusteredShadingSceneRenderer.h"

namespace ln {

#if 0
//==============================================================================
// LightComponent

//------------------------------------------------------------------------------
LightComponent::LightComponent()
	: VisualComponent()
	, m_lightInfo(nullptr)
	, m_enabled(true)
	, m_spotAngle(Math::PI * 0.25f)
	//, m_viewMatrix(Matrix::Identity)
	//, m_projMatrix(Matrix::Identity)
	//, m_viewProjMatrix(Matrix::Identity)
{
}

//------------------------------------------------------------------------------
LightComponent::~LightComponent()
{
}

//------------------------------------------------------------------------------
void LightComponent::init(LightType type)
{
    VisualComponent::init();
	m_lightInfo = makeRef<detail::DynamicLightInfo>();
	//m_lightInfo->m_type = type;
	//m_lightInfo->m_color.set(1.0f, 1.0f, 1.0f, 1.0f);
	//m_lightInfo->m_ambient.set(1.0f, 1.0f, 1.0f, 1.0f);
	//m_lightInfo->m_specular.set(1.0f, 1.0f, 1.0f, 1.0f);
	//m_lightInfo->m_shadowZFar = 1000.0f;
}

//------------------------------------------------------------------------------
//void LightComponent::updateMatrices(/*const Size& viewSize*/)
//{
//	const Matrix& worldMatrix = getOwnerObject()->transform.getWorldMatrix();
//
//	// 正面方向
//	Vector3 direction = Vector3::transformCoord(Vector3(0, 0, 1), worldMatrix);
//
//	// 注視点
//	Vector3 lookAt = worldMatrix.getPosition() + direction;
//
//	// ビュー行列
//	Vector3 up = Vector3(0, 1, 0);
//	//m_viewMatrix = Matrix::makeLookAtLH(worldMatrix.getPosition(), lookAt, up);
//
//	// プロジェクション行列の更新
//	// TODO: 視野角とnear,far
//	// https://sites.google.com/site/mmereference/home/Annotations-and-Semantics-of-the-parameter/2-1-geometry-translation
//	//m_projMatrix = Matrix::makePerspectiveFovLH(Math::PI / 4.0f, viewSize.width / viewSize.height, 0.01f, 1000.0f);
//
//	//m_worldViewProjMatrix = getMatrix() * m_viewMatrix * m_projMatrix;
//	//m_viewProjMatrix = m_viewMatrix * m_projMatrix;
//
//	m_lightInfo->m_direction = direction;
//
//	//m_viewMatrixI = Matrix::makeInverse(m_viewMatrix);
//	//m_projMatrixI = Matrix::makeInverse(m_projMatrix);
//	//m_viewProjMatrixI = Matrix::makeInverse(m_viewProjMatrix);
//	//m_viewMatrixT = Matrix::makeTranspose(m_viewMatrix);
//	//m_projMatrixT = Matrix::makeTranspose(m_projMatrix);
//	//m_viewProjMatrixT = Matrix::makeTranspose(m_viewProjMatrix);
//	//m_viewMatrixIT = Matrix::makeTranspose(m_viewMatrixI);
//	//m_projMatrixIT = Matrix::makeTranspose(m_projMatrixI);
//	//m_viewProjMatrixIT = Matrix::makeTranspose(m_viewProjMatrixI);
//}
//
//------------------------------------------------------------------------------
void LightComponent::onPreRender(DrawList* context)
{
	if (m_enabled)
	{
		//updateMatrices();
		const Matrix& t = getOwnerObject()->transform.getWorldMatrix();
		m_lightInfo->m_position = t.getPosition();
		m_lightInfo->m_direction = t.getFront();
		context->addDynamicLightInfo(m_lightInfo);
	}
}
#endif

//==============================================================================
// AmbientLightComponent

LN_OBJECT_IMPLEMENT(AmbientLightComponent, VisualComponent) {}

AmbientLightComponent::AmbientLightComponent()
	: m_color(Color::White)
	, m_intensity(0.5f)
	, m_enabled(true)
{
}

AmbientLightComponent::~AmbientLightComponent()
{
}

void AmbientLightComponent::init()
{
	VisualComponent::init();
}

void AmbientLightComponent::onPrepareRender(RenderingContext* context)
{
	if (m_enabled)
	{
		context->addAmbientLight(m_color, m_intensity);
	}
}

//==============================================================================
// HemisphereLightComponent

LN_OBJECT_IMPLEMENT(HemisphereLightComponent, VisualComponent) {}

HemisphereLightComponent::HemisphereLightComponent()
	: m_skyColor(Color::White)
	, m_groundColor(Color::White)
	, m_intensity(0.5f)
	, m_enabled(true)
{
}

HemisphereLightComponent::~HemisphereLightComponent()
{
}

void HemisphereLightComponent::init()
{
	VisualComponent::init();
}

void HemisphereLightComponent::onPrepareRender(RenderingContext* context)
{
	if (m_enabled)
	{
		context->addHemisphereLight(m_skyColor, m_groundColor, m_intensity);
	}
}

//==============================================================================
// DirectionalLightComponent

LN_OBJECT_IMPLEMENT(DirectionalLightComponent, VisualComponent) {}

DirectionalLightComponent::DirectionalLightComponent()
	: m_color(Color::White)
	, m_intensity(0.5f)
	, m_enabled(true)
{
}

DirectionalLightComponent::~DirectionalLightComponent()
{
}

void DirectionalLightComponent::init()
{
	VisualComponent::init();
}

void DirectionalLightComponent::setShadowCast(bool enabled)
{
    LN_NOTIMPLEMENTED();
	//if (enabled)
	//{
	//	m_shadowCasterPass = makeObject<detail::ShadowCasterPass>();
	//}
	//else
	//{
	//	m_shadowCasterPass.safeRelease();
	//}
}

bool DirectionalLightComponent::isShadowCast() const
{
	LN_NOTIMPLEMENTED();
	return false;
	//return m_shadowCasterPass != nullptr;
}

void DirectionalLightComponent::onPrepareRender(RenderingContext* context)
{
	if (m_enabled)
	{
        const Matrix& t = worldObject()->worldMatrix();
		context->addDirectionalLight(m_color, m_intensity, t.front());

		//if (m_shadowCasterPass != nullptr)
		//{
		//	m_shadowCasterPass->view.makePerspective(
		//		m_lightInfo->m_position, m_lightInfo->m_direction,
		//		Math::PI / 2.0f,
		//		//m_shadowCasterPass->m_shadowMap->getSize().toFloatSize(),
		//		Size(1024.0/8, 1024.0 / 8),	// TODO: LightMapSize
		//		0.5f, 100.0f);	// TODO: clip range
		//}
	}
}

//==============================================================================
// PointLightComponent

LN_OBJECT_IMPLEMENT(PointLightComponent, VisualComponent) {}

PointLightComponent::PointLightComponent()
	: m_color(Color::White)
	, m_intensity(1.0f)
	, m_range(10.0f)
	, m_attenuation(1.0f)
	, m_enabled(true)
{
}

PointLightComponent::~PointLightComponent()
{
}

void PointLightComponent::init()
{
	VisualComponent::init();
}

void PointLightComponent::onPrepareRender(RenderingContext* context)
{
	if (m_enabled)
	{
		const Matrix& t = worldObject()->worldMatrix();
		context->addPointLight(m_color, m_intensity, t.position(), m_range, m_attenuation);
	}
}

//==============================================================================
// SpotLightComponent

LN_OBJECT_IMPLEMENT(SpotLightComponent, VisualComponent) {}

SpotLightComponent::SpotLightComponent()
	: m_color(Color::White)
	, m_intensity(1.0f)
	, m_range(10.0f)
	, m_attenuation(1.0f)
	, m_spotAngle(Math::PI / 3)
	, m_spotPenumbra(0.1f)
	, m_enabled(true)
{
}

SpotLightComponent::~SpotLightComponent()
{
}

void SpotLightComponent::init()
{
	VisualComponent::init();
}

void SpotLightComponent::onPrepareRender(RenderingContext* context)
{
	if (m_enabled)
	{
        const Matrix& t = worldObject()->worldMatrix();
		context->addSpotLight(
			m_color, m_intensity, t.position(), t.front(),
			m_range, m_attenuation, m_spotAngle, m_spotPenumbra);
	}
}

} // namespace ln