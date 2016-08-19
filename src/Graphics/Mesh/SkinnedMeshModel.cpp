﻿
#include "../Internal.h"
#include <Lumino/Graphics/Mesh.h>
#include <Lumino/Graphics/Mesh/SkinnedMeshModel.h>
#include "../GraphicsManager.h"
#include "../../Modeling/PmxSkinnedMesh.h"

LN_NAMESPACE_BEGIN


static void NormalizeEular(Vector3* eulers)
{
	float& x = eulers->x;
	float& y = eulers->y;
	float& z = eulers->z;
	if (x < -Math::PI || Math::PI < x)
	{
		if (x > 0)
		{
			x -= Math::PI * 2;
		}
		else
		{
			x += Math::PI * 2;
		}
	}
	if (y < -Math::PI*0.5f || Math::PI*0.5f < y)
	{
		if (y > 0)
		{
			y -= Math::PI * 2;
		}
		else
		{
			y += Math::PI * 2;
		}
	}
	if (z < -Math::PI || Math::PI < z)
	{
		if (z > 0)
		{
			z -= Math::PI * 2;
		}
		else
		{
			z += Math::PI * 2;
		}
	}
}

class CCDIK
{
public:

	SkinnedMeshModel* owner;

	void UpdateTransform()
	{
		for (SkinnedMeshBone* ikBone : owner->m_ikBoneList)
		{
			UpdateEachIKBoneTransform(ikBone->m_ikInfo);
		}
	}

	void UpdateEachIKBoneTransform(PmxIKResource* ik)
	{
		for (int iCalc = 0; iCalc < ik->LoopCount; iCalc++)
		{
			SkinnedMeshBone* ikBone = owner->m_allBoneList[ik->IKBoneIndex];			// IK ボーン (IK 情報を持つボーン。目標地点)
			SkinnedMeshBone* effector = owner->m_allBoneList[ik->IKTargetBoneIndex];	// IK ターゲットボーン (エフェクタ。IKに向くべきボーンたちの中の先頭ボーン)

			IKloop(ik, ikBone, effector);
		}
	}
	void IKloop(PmxIKResource* ik, SkinnedMeshBone* ikBone, SkinnedMeshBone* effector)
	{
		Vector3 TargetGlobalPos = Vector3::TransformCoord(ikBone->GetCore()->OrgPosition, ikBone->GetCombinedMatrix());

		for (int iLink = 0; iLink < ik->IKLinks.GetCount(); ++iLink)
		{
			PmxIKResource::IKLink& ikLink = ik->IKLinks[iLink];
			SkinnedMeshBone* ikLinkBone = owner->m_allBoneList[ikLink.LinkBoneIndex];

			// ワールド座標系から注目ノードの局所座標系への変換
			// (IKリンク基準のローカル座標系へ変換する行列)
			Matrix toLinkLocal = Matrix::MakeInverse(ikLinkBone->GetCombinedMatrix());

			Vector3 effectorPos = Vector3::TransformCoord(effector->GetCore()->OrgPosition, effector->GetCombinedMatrix() * toLinkLocal);
			Vector3 link2Effector = Vector3::SafeNormalize(effectorPos - ikLinkBone->GetCore()->OrgPosition);
		
			Vector3 targetPos = Vector3::TransformCoord(TargetGlobalPos, toLinkLocal);
			Vector3 link2Target = Vector3::SafeNormalize(targetPos - ikLinkBone->GetCore()->OrgPosition);

			IKLinkCalc(ikLink, ikLinkBone, link2Effector, link2Target, ik->IKRotateLimit);
		}
	}

	void IKLinkCalc(PmxIKResource::IKLink& ikLink, SkinnedMeshBone* ikLinkBone, const Vector3& link2Effector, const Vector3& link2Target, float RotationLimited)
	{
		// 回転角度を求める
		float dot = Vector3::Dot(link2Effector, link2Target);
		if (dot > 1.0f) dot = 1.0f;
		float rotationAngle = Math::Clamp(acosf(dot), -RotationLimited, RotationLimited);
		if (Math::IsNaN(rotationAngle)) return;
		if (rotationAngle <= 1.0e-3f) return;

		// 回転軸を求める
		Vector3 rotationAxis = Vector3::Cross(link2Effector, link2Target);

		// 軸を中心として回転する行列を作成する
		Quaternion rotation = Quaternion(rotationAxis, rotationAngle);
		rotation.Normalize();
		ikLinkBone->GetLocalTransformPtr()->rotation = rotation * ikLinkBone->GetLocalTransformPtr()->rotation;

		// 回転量制限
		ikLinkBone->GetLocalTransformPtr()->rotation = RestrictRotation(ikLink, ikLinkBone->GetLocalTransformPtr()->rotation);

		ikLinkBone->UpdateGlobalTransform(true);
	}

	Quaternion RestrictRotation(const PmxIKResource::IKLink& ikLink, const Quaternion& localRot)
	{
		if (!ikLink.IsRotateLimit) return localRot;

		RotationOrder type;
		bool locked;
		Vector3 euler;

		// まずオイラー角に分解する。
		// 分解の試行順序は XYZ が一番最初でなければならない (Love&Joy モーションで破綻する)
		type = RotationOrder::XYZ;
		euler = localRot.ToEulerAngles(type, &locked);
		if (locked)
		{
			type = RotationOrder::YZX;
			euler = localRot.ToEulerAngles(type, &locked);
			if (locked)
			{
				type = RotationOrder::ZXY;
				euler = localRot.ToEulerAngles(type, &locked);
				if (locked)
				{
					LN_CHECK_STATE(0);	// あり得ないはずだが…
				}
			}
		}

		// 角度修正
		NormalizeEular(&euler);
		euler.Clamp(ikLink.MinLimit, ikLink.MaxLimit);

		// 戻す
		return Quaternion::MakeFromRotationMatrix(Matrix::MakeRotationEulerAngles(euler, type));
	}
};



//==============================================================================
// SkinnedMeshModel
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(SkinnedMeshModel, Object);

//------------------------------------------------------------------------------
SkinnedMeshModel::SkinnedMeshModel()
	: m_meshResource(nullptr)
	, m_materials(nullptr)
	, m_allBoneList()
	, m_rootBoneList()
	, m_skinningMatrices()
	, m_skinningMatricesTexture(nullptr)
	, m_animator(nullptr)
{
}

//------------------------------------------------------------------------------
SkinnedMeshModel::~SkinnedMeshModel()
{
}

//------------------------------------------------------------------------------
void SkinnedMeshModel::Initialize(GraphicsManager* manager, PmxSkinnedMeshResource* sharingMesh)
{
	LN_CHECK_ARG(manager != nullptr);
	LN_CHECK_ARG(sharingMesh != nullptr);

	// メッシュ(バッファ類)は共有する
	m_meshResource = sharingMesh;

	// マテリアルはコピーする
	// TODO: コピー有無のフラグがあったほうがいいかも？
	int count = m_meshResource->materials.GetCount();
	m_materials = RefPtr<MaterialList>::MakeRef();
	m_materials->Resize(count);
	for (int i = 0; i < count; ++i)
	{
		m_materials->SetAt(i, m_meshResource->materials.GetAt(i)->MakeCommonMaterial());
	}

	//---------------------------------------------------------
	// Bone のインスタンス化
	int boneCount = m_meshResource->bones.GetCount();
	if (boneCount > 0)
	{
		m_allBoneList.Resize(boneCount);
		// まずは Bone を作る
		for (int i = 0; i < boneCount; i++)
		{
			m_allBoneList[i] = SkinnedMeshBonePtr::MakeRef();
			m_allBoneList[i]->Initialize(m_meshResource->bones[i]);
			
			// IK ボーンを集める
			if (m_meshResource->bones[i]->IsIK)
			{
				m_ikBoneList.Add(m_allBoneList[i]);
			}
		}
		// 次に子と親を繋げる
		for (int i = 0; i < boneCount; i++)
		{
			int parentIndex = m_meshResource->bones[i]->ParentBoneIndex;
			if (0 <= parentIndex && parentIndex < boneCount)
			{
				m_allBoneList[parentIndex]->AddChildBone(m_allBoneList[i]);
			}
			else
			{
				m_rootBoneList.Add(m_allBoneList[i]);	// 親がいない。ルートボーンとして覚えておく
			}
		}
		for (SkinnedMeshBone* rootBone : m_rootBoneList)
		{
			rootBone->PostInitialize(this, 0);
		}
		for (PmxIKResource* ik : m_meshResource->iks)
		{
			m_allBoneList[ik->IKBoneIndex]->m_ikInfo = ik;
		}

		// ボーン行列を書き込むところを作る
		m_skinningMatrices.Resize(boneCount);
		m_skinningMatricesTexture = RefPtr<Texture2D>::MakeRef();
		m_skinningMatricesTexture->Initialize(manager, SizeI(4, boneCount), TextureFormat::R32G32B32A32_Float, false);	// TODO: Dynamic、NoManaged

		// アニメーション管理
		m_animator = RefPtr<Animator>::MakeRef();
		m_animator->Create(this);
	}


	struct BoneComparer
	{
		int boneCount;

		bool operator () (SkinnedMeshBone* x, SkinnedMeshBone* y) const
		{
			//後であればあるほどスコアが大きくなるように計算する
			int xScore = 0;
			int yScore = 0;
			if (x->GetCore()->TransformAfterPhysics)
			{
				xScore += boneCount * boneCount;
			}
			if (y->GetCore()->TransformAfterPhysics)
			{
				yScore += boneCount * boneCount;
			}
			xScore += boneCount * x->m_depth;
			yScore += boneCount * y->m_depth;
			xScore += x->GetCore()->GetBoneIndex();
			yScore += y->GetCore()->GetBoneIndex();
			return xScore < yScore;
		}

	} cmp;
	cmp.boneCount = m_allBoneList.GetCount();

	std::sort(m_ikBoneList.begin(), m_ikBoneList.end(), cmp);
}

//------------------------------------------------------------------------------
// 姿勢更新① (ユーザー更新の前に確定する必要があるもの)
//		・ボーンのグローバル行列更新
//		・剛体更新 (フレーム位置を剛体位置へ)
void SkinnedMeshModel::PreUpdate()
{
	UpdateBoneTransformHierarchy();
}

//------------------------------------------------------------------------------
// 姿勢更新② (ユーザー更新の後、描画の前に行う必要があるもの)
//		・モーフ更新
//		・IK更新
//		・剛体更新 (剛体位置をフレーム位置へ)
//		・スキニング行列の作成
void SkinnedMeshModel::PostUpdate()
{
	// IK 更新
	UpdateIK();

	// スキニング行列の作成
	UpdateSkinningMatrices();
}

//------------------------------------------------------------------------------
void SkinnedMeshModel::UpdateBoneTransformHierarchy()
{
	for (SkinnedMeshBone* bone : m_rootBoneList)
	{
		bone->UpdateGlobalTransform(true);
	}
}

//------------------------------------------------------------------------------
void SkinnedMeshModel::UpdateSkinningMatrices()
{
	// スキニング行列の作成
	for (int i = 0; i < m_allBoneList.GetCount(); i++)
	{
		/*
			初期姿勢は、スキニングしなくても同じ姿勢。
			つまり、頂点スキニングで初期姿勢にしたいときは Matrix::Identity のボーン行列を渡す。

			ボーンは最初からオフセットが入ってるけど、
			それをスキニングに適用すると姿勢が崩れてしまう。
			そのため、初期オフセットを打ち消す処理が必要。それが GetInitialTranstormInv()。

			ID3DXSkinInfo::GetBoneOffsetMatrix() で
			取得できる行列 (SkinnedMeshサンプルの D3DXMESHCONTAINER_DERIVED::pBoneOffsetMatrices) がこれにあたるものっぽい。
			サンプルでも描画の直前に対象ボーン行列にこの行列を乗算している。
		*/
		//m_skinningMatrices[i] = m_allBoneList[i]->GetCore()->GetInitialTranstormInv();
		m_skinningMatrices[i] = m_allBoneList[i]->GetCombinedMatrix();
	}

	// スキニングテクスチャ更新
	if (!m_skinningMatricesTexture.IsNull())
	{
		m_skinningMatricesTexture->SetSubData(Point(0, 0), &m_skinningMatrices[0]);
	}

	// 全てのローカルトランスフォームをリセットする
	//		リセットしておかないと、IKで問題が出る。
	//		(IKはその時点のLocalTransformに対して処理を行うため、回転角度がどんどん増えたりする)
	//		なお、一連の更新の最後で行っているのは、アニメーションからの更新を外部で行っているため。
	// TODO: できれば一連の処理の中で必ず通るところに移動したい
	for (SkinnedMeshBone* bone : m_allBoneList)
	{
		bone->ResetLocalTransform();
	}
}


//------------------------------------------------------------------------------
void SkinnedMeshModel::UpdateIK()
{
	CCDIK ik;
	ik.owner = this;
	ik.UpdateTransform();
}

//------------------------------------------------------------------------------
int SkinnedMeshModel::GetAnimationTargetAttributeCount() const
{
	return m_allBoneList.GetCount();
}

//------------------------------------------------------------------------------
IAnimationTargetAttribute* SkinnedMeshModel::GetAnimationTargetAttribute(int index)
{
	return m_allBoneList[index].Get();
}

//==============================================================================
// SkinnedMeshBone
//==============================================================================
LN_TR_REFLECTION_TYPEINFO_IMPLEMENT(SkinnedMeshBone, Object);

//------------------------------------------------------------------------------
SkinnedMeshBone::SkinnedMeshBone()
	: m_core(nullptr)
	, m_parent(nullptr)
	, m_children()
	, m_localTransform()
	, m_combinedMatrix()
	, m_depth(0)
	, m_ikInfo(nullptr)
{
}

//------------------------------------------------------------------------------
SkinnedMeshBone::~SkinnedMeshBone()
{
}

//------------------------------------------------------------------------------
void SkinnedMeshBone::Initialize(PmxBoneResource* boneResource)
{
	m_core = boneResource;
}

//------------------------------------------------------------------------------
void SkinnedMeshBone::PostInitialize(SkinnedMeshModel* owner, int depth)
{
	m_depth = depth;

	for (SkinnedMeshBone* bone : m_children)
	{
		bone->PostInitialize(owner, m_depth + 1);
	}
}

//------------------------------------------------------------------------------
PmxBoneResource* SkinnedMeshBone::GetCore() const
{
	return m_core;
}

//------------------------------------------------------------------------------
void SkinnedMeshBone::AddChildBone(SkinnedMeshBone* bone)
{
	LN_CHECK_ARG(bone != nullptr);
	LN_CHECK_ARG(bone->m_parent == nullptr);
	m_children.Add(bone);
	bone->m_parent = this;
}

//------------------------------------------------------------------------------
void SkinnedMeshBone::UpdateGlobalTransform(bool hierarchical)
{
	m_combinedMatrix =
		Matrix::MakeTranslation(-m_core->OrgPosition) *
		Matrix::MakeRotationQuaternion(m_localTransform.rotation) *
		Matrix::MakeTranslation(m_localTransform.translation) *
		Matrix::MakeTranslation(m_core->OrgPosition);

	// 親行列と結合
	Matrix parentRotation;
	if (m_parent != nullptr)
	{
		m_combinedMatrix *= m_parent->GetCombinedMatrix();
	}

	// 子ボーン更新
	if (hierarchical)
	{
		for (SkinnedMeshBone* bone : m_children)
		{
			bone->UpdateGlobalTransform(hierarchical);
		}
	}
}

//------------------------------------------------------------------------------
const String& SkinnedMeshBone::GetAnimationTargetName() const
{
	return m_core->Name;
}

//------------------------------------------------------------------------------
void SkinnedMeshBone::SetAnimationTargetValue(ValueType type, const void* value)
{
	LN_CHECK_ARG(type == ValueType_SQTTransform);
	m_localTransform = *((SQTTransform*)value);
}

LN_NAMESPACE_END
