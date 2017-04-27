﻿
#include "../Internal.h"
#include "RenderingPass.h"
#include <Lumino/Graphics/GraphicsContext.h>
#include <Lumino/Scene/Camera.h>
#include <Lumino/Scene/Light.h>
#include <Lumino/Scene/VisualNode.h>
#include <Lumino/Scene/SceneGraph.h>
#include "../EngineDiagCore.h"
#include "SceneGraphManager.h"

LN_NAMESPACE_BEGIN
LN_NAMESPACE_SCENE_BEGIN

//==============================================================================
// MaterialList2
//==============================================================================

//------------------------------------------------------------------------------
MaterialList2::MaterialList2()
{
}

//------------------------------------------------------------------------------
MaterialList2::~MaterialList2()
{
}

//------------------------------------------------------------------------------
void MaterialList2::Initialize(int subMaterialCount, bool createMainMaterial)
{
	Resize(subMaterialCount);
	for (int i = 0; i < subMaterialCount; ++i)
	{
		auto m = RefPtr<Material>::MakeRef();	// TODO
		SetAt(i, m);
	}

	if (createMainMaterial)
	{
		m_mainMaterial = RefPtr<Material>::MakeRef();	// TODO
	}
}

//------------------------------------------------------------------------------
void MaterialList2::CopyShared(MaterialList* srcList, bool createMainMaterial)
{
	if (LN_CHECK_ARG(srcList != nullptr)) return;

	Resize(srcList->GetCount());
	for (int i = 0; i < srcList->GetCount(); ++i)
	{
		SetAt(i, srcList->GetAt(i));
	}


	if (createMainMaterial)
	{
		m_mainMaterial = RefPtr<Material>::MakeRef();
	}
	else if (GetCount() > 0)
	{
		m_mainMaterial = GetAt(0);
	}
}

//------------------------------------------------------------------------------
Material* MaterialList2::GetMainMaterial() const
{
	return m_mainMaterial;
}


//==============================================================================
// VisualNode
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(VisualNode, SceneNode);

//------------------------------------------------------------------------------
VisualNode::VisualNode()
{
}

//------------------------------------------------------------------------------
VisualNode::~VisualNode()
{
}

//------------------------------------------------------------------------------
void VisualNode::Initialize(SceneGraph* owner, int subsetCount)
{
	SceneNode::Initialize(owner);
	m_materialList = RefPtr<MaterialList2>::MakeRef();
	m_materialList->Initialize(subsetCount, true);	// TODO: 今はとりあえず必ず mainMaterial 有り (メモリ効率悪い)
}

//------------------------------------------------------------------------------
Material* VisualNode::GetMainMaterial() const
{
	return m_materialList->GetMainMaterial();
}

//------------------------------------------------------------------------------
tr::ReflectionObjectList<Material*>* VisualNode::GetMaterials() const
{
	return m_materialList;
}

//------------------------------------------------------------------------------
void VisualNode::SetOpacity(float value, int subsetIndex)
{
	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
	// TODO: サブマテリアルの設定
	//m_materialList->GetMainMaterial()->SetOpacity(opacity);
	m_builtinEffectData.SetOpacity(value);
}
float VisualNode::GetOpacity() const
{
	return m_builtinEffectData.GetOpacity();
}

void VisualNode::SetColorScale(const Color& value, int subsetIndex)
{
	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
	// TODO: サブマテリアルの設定
	//m_materialList->GetMainMaterial()->SetColorScale(color);
	m_builtinEffectData.SetColorScale(value);
}
void VisualNode::SetColorScale(float r, float g, float b, float a, int subsetIndex)
{
	SetColorScale(Color(r, g, b, a));
}
void VisualNode::SetColor(const Color32& color)
{
	SetColorScale(Color(color));
}
void VisualNode::SetColor(int r, int g, int b, int a)
{
	SetColor(Color32(r, g, b, a));
}
const Color& VisualNode::GetColorScale() const
{
	return m_builtinEffectData.GetColorScale();
}
void VisualNode::SetBlendColor(const Color& color, int subsetIndex)
{
	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
	// TODO: サブマテリアルの設定
	//m_materialList->GetMainMaterial()->SetBlendColor(color);
	m_builtinEffectData.SetBlendColor(color);
}
void VisualNode::SetTone(const ToneF& tone, int subsetIndex)
{
	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
	// TODO: サブマテリアルの設定
	//m_materialList->GetMainMaterial()->SetTone(tone);
	m_builtinEffectData.SetTone(tone);
}
void VisualNode::SetShader(Shader* value, int subsetIndex)
{
	if (LN_CHECK_STATE(m_materialList->GetMainMaterial() != nullptr)) return;
	// TODO: サブマテリアルの設定
	//m_materialList->GetMainMaterial()->SetShader(shader);
	m_builtinEffectData.SetShader(value);
}

//------------------------------------------------------------------------------
void VisualNode::SetBlendMode(BlendMode mode) { GetMainMaterial()->SetBlendMode(mode); }

//------------------------------------------------------------------------------
//BlendMode VisualNode::GetBlendMode() const { return GetMainMaterial()->GetBlendMode(); }

//------------------------------------------------------------------------------
void VisualNode::SetCullingMode(CullingMode mode) { GetMainMaterial()->SetCullingMode(mode); }

//------------------------------------------------------------------------------
//CullingMode VisualNode::GetCullingMode() const { return GetMainMaterial()->GetCullingMode(); }

//------------------------------------------------------------------------------
void VisualNode::SetDepthTestEnabled(bool enabled) { GetMainMaterial()->SetDepthTestEnabled(enabled); }

//------------------------------------------------------------------------------
//bool VisualNode::IsDepthTestEnabled() const { return GetMainMaterial()->IsDepthTestEnabled(); }

//------------------------------------------------------------------------------
void VisualNode::SetDepthWriteEnabled(bool enabled) { GetMainMaterial()->SetDepthWriteEnabled(enabled); }

//------------------------------------------------------------------------------
//bool VisualNode::IsDepthWriteEnabled() const { return GetMainMaterial()->IsDepthWriteEnabled(); }

//------------------------------------------------------------------------------
void VisualNode::UpdateFrameHierarchy(SceneNode* parent, float deltaTime)
{
	// TODO: 描画関係のデータは UpdateFrame でやるべきではないような気もする。
	//m_materialList->UpdateMaterialInstances(m_ownerSceneGraph);

	SceneNode::UpdateFrameHierarchy(parent, deltaTime);
}

//------------------------------------------------------------------------------
detail::Sphere VisualNode::GetBoundingSphere()
{
	const detail::Sphere s{ Vector3::Zero, -1 };
	return s;
}

//------------------------------------------------------------------------------
Shader* VisualNode::GetPrimaryShader() const
{
	// TODO: main が無ければ [0] のをつかう
	return m_materialList->GetMainMaterial()->GetShader();
}

LN_NAMESPACE_SCENE_END
LN_NAMESPACE_END
