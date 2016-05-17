﻿
#include "../Internal.h"
#include "SceneGraphManager.h"
#include <Lumino/Scene/Sprite3D.h>

LN_NAMESPACE_BEGIN
LN_NAMESPACE_SCENE_BEGIN

//==============================================================================
// Sprite3D
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(Sprite3D, Sprite);

//------------------------------------------------------------------------------
Sprite3DPtr Sprite3D::Create()
{
	auto obj = Sprite3DPtr::MakeRef();
	obj->Initialize(SceneGraphManager::Instance->GetDefault3DSceneGraph());
	return obj;
}

//------------------------------------------------------------------------------
Sprite3DPtr Sprite3D::Create(float width, float height)
{
	auto obj = Sprite3DPtr::MakeRef();
	obj->Initialize(SceneGraphManager::Instance->GetDefault3DSceneGraph());
	obj->SetSize(SizeF(width, height));
	return obj;
}

//------------------------------------------------------------------------------
Sprite3DPtr Sprite3D::Create(float width, float height, Texture* texture)
{
	auto obj = Sprite3DPtr::MakeRef();
	obj->Initialize(SceneGraphManager::Instance->GetDefault3DSceneGraph());
	obj->SetSize(SizeF(width, height));
	obj->SetTexture(texture);
	return obj;
}

//------------------------------------------------------------------------------
Sprite3D::Sprite3D()
{
}

//------------------------------------------------------------------------------
Sprite3D::~Sprite3D()
{
}

//------------------------------------------------------------------------------
void Sprite3D::Initialize(SceneGraph* owner)
{
	Sprite::Initialize(owner, SpriteCoord_RZ);
}

LN_NAMESPACE_SCENE_END
LN_NAMESPACE_END
