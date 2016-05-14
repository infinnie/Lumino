/*
	[2015/5/11] SceneGraph �ɍ��킹����ނ� Material �͂ǂ��\������H

		- �R���|�[�l���g
		�l�� get �������񌟍��ɂȂ�B���t���[������͂�����ƁE�E�E�B
		���[�U�[���l�ɃA�N�Z�X����Ƃ���������Ńv���p�e�B�����w�肷��B�^�C�v�Z�[�t����Ȃ��B

		- �p��
			�����I�ɂ͂���ōs�������E�E�E�B

			����Unity�݂����ȃG�f�B�^������͂���Ȃ�
			- �I�u�W�F�N�g�ɃR���|�[�l���g�Ƃ��Ēǉ�����}�e���A���͂����܂Ńf�[�^���f���N���X�ƂȂ�B
			- Forward�p�}�e���A���AMMD�p�}�e���A���A�ȂǁB������͊�{�I�ɒl�� map �Ŏ��B
			- Node �����Ƃ��A���̃f�[�^���f������A���݂� Scene ��ނɍ��킹�� Material �N���X�����B
			  ���� Material �N���X�ɂ͊�{�I�ɂǂ�ȃf�[�^���f���N���X����͂��Ă��悢���Ƃɂ���B

			�Ȃ��AScene �� Material ���Ή�����s����ANode �͍쐬���ɕK�����炩�� SceneGraph �ɑ����邱�ƂɂȂ�B


	[2016/5/9] 
		- ����܂ł̃��K�V�[�ȃv���p�e�B (Diffuse/Ambient/Emissive/Speculer �Ȃ�)
		- �x���V�F�[�f�B���O���L�̃v���p�e�B
		- Lumino ���L�̃Q�[�������v���p�e�B (BlendColor/Tone �Ȃ�)
		- MMD ���L�̃v���p�e�B (�X�t�B�A�}�b�v �Ȃ�)
		
		
		
		�V�F�[�_�̎��������̃v���p�e�B (��ɃZ�}���e�B�N�X�Ƃ����Ă����) �́A
		SceneGraph ����l���Ƃ��Ă���B
		
		��{�I�ɂ́u�ǂ�ȃv���p�e�B���K�v���H�v�̓}�e���A�����ł͂Ȃ��A�V�F�[�_������
		�Z�}���e�B�N�X�ȂǂŎw�肷�邱�ƂɂȂ�B
		
		MMD �p�V�[�����g���Ă���ꍇ�Ȃǂ́A�v���p�e�B��C++���Ŋo���Ă����K�v������B
		�V�F�[�_�v���p�e�B�Ɋo���Ă����A���ƁA�V�F�[�_��؂�ւ����Ƃ��� Diffuse �l�Ȃǂ�������B
		
		���܂܂Ŏg���Ă��� SceneShader �� Material �̖����ƂȂ�B
		
		BlendColor/Tone �Ȃǂ͑S�Ẵ}�e���A���ɉe������B
		
		
		
		
		�Z�}���e�B�N�X
		
		SetVector("_Color", "DIFFUSE", color);
		�Ƃ��ɂ���ꍇ�A������ map �ɕϐ����ƒl���o���Ă����K�v������B
		�ł��A�V�F�[�_�����Z�}���e�B�N�X�ŗv�����Ă���ꍇ�A�ǂ�ȃZ�}���e�B�N�X�ɂ��Ή��ł���悤��
		�l�X�Ȓl�� map ������Ă����Ȃ���΂Ȃ�Ȃ��B(���ɕϊ��s�񂪑����Bworld, view, proj �̓]�u�A�t�A�Ȃ�)
		����܂łǂ���AApply ����Ƃ�������őS�ϐ����Ȃ߂āA�ϐ����Z�}���e�B�N�X�ŗv�����Ă���l��
		SceneGraph �Ȃ� Node �Ȃ� Material �Ȃ肩����o���̂������������I�ɂ悢�B
		
		
		���L�̃v���p�e�B�����}�e���A���͔h���N���X�ɂ���H
		�S��1�̃}�e���A���N���X�Ɏ�������Ɩ��ʂ���������B
		�܂��A�����o�ϐ��Ɏ������� map �Ǘ��ɂ���Ɛݒ葤���Z�}���e�B�N�X���Ȃǂ̃��[�������K�v������B�Ǘ����������G�B
		�܂��͔h���N���X�Ƃ��č���Ă݂�B
		
		
		SubMaterial �͕K��1�ȏ����ׂ��H
			���������A
			- SubMaterial �� 3D���f���ɂ������� Asset ���烍�[�h����鋤�L�}�e���A���B��{�I�� Static�B
			- MainMaterial �� Node �ɂ��������[�g�̃}�e���A���B��{�I�� Dynamic�B
			�Ɛ�������ق����킩��₷���B
			��������ƁASub[0] = Main �ɂ��悤�Ƃ���̂͊ԈႢ�ȋC������B
			
			SubMaterial 1�ȏ�AMainMaterial ���� �� ���S��static���b�V���B
			SubMaterial 0�AMainMaterial �L�� �� Asset �g�킸�A�R�[�h��� Sprite::Create() �Ƃ������ꍇ�B
			SubMaterial 0�AMainMaterial �����A�Ƃ����p�^�[�������肦��B���̏ꍇ�̓f�t�H���g�̃}�e���A�����g���悤�ɂ���B
			
			
		Color �� uint �Ȃ̂� opacity �� float?
			WPF �͂���Ȃ��񂶁B
		
		
		
		
		UE4 - UMaterial
		https://docs.unrealengine.com/latest/INT/API/Runtime/Engine/Materials/UMaterial/index.html
		
		Uity - Material
		http://docs.unity3d.com/ja/current/ScriptReference/Material.html
		
	
	RenderState �� Node �Ɏ�������ׂ��� Material �Ɏ�������ׂ���
		- Unity �� Shader��Pass����
		- Ogre �� Material
		
		Material �Ɏ�������ꍇ�A�u�����Ă��Ȃ��v�Ƃ��������K�v�B
		�قƂ�ǂ̏ꍇ�� Node �Ɏ������Ă����Ηp�͑����B
*/

#include "Internal.h"
#include <Lumino/Graphics/Texture.h>
#include <Lumino/Graphics/Shader.h>
#include <Lumino/Scene/Material.h>
#include "MME/MmdMaterial.h"	// TODO

LN_NAMESPACE_BEGIN

//==============================================================================
// Material
//==============================================================================

//------------------------------------------------------------------------------
Material2::Material2(int materialTypeId)
	: m_materialTypeId(materialTypeId)
	, m_opacity(1.0f)
	, m_colorScale(1, 1, 1, 1)
	, m_blendColor(0, 0, 0, 0)
	, m_tone()
	, m_texture(nullptr)
	, m_shader(nullptr)
{
}

//------------------------------------------------------------------------------
Material2::~Material2()
{
	LN_SAFE_RELEASE(m_texture);
	LN_SAFE_RELEASE(m_shader);
}

//------------------------------------------------------------------------------
void Material2::SetTexture(Texture* texture)
{
	LN_REFOBJ_SET(m_texture, texture);
}

//------------------------------------------------------------------------------
void Material2::SetShader(Shader* shader)
{
	LN_REFOBJ_SET(m_shader, shader);
}



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
		auto m = RefPtr<MmdMaterial>::MakeRef();	// TODO
		SetAt(i, m);
	}

	if (createMainMaterial)
	{
		m_mainMaterial = RefPtr<MmdMaterial>::MakeRef();	// TODO
	}
}

//------------------------------------------------------------------------------
void MaterialList2::UpdateMaterialInstances()
{
	// m_mainMaterial �͐e�Ƃ��Ďg����H
	Material2* parent = nullptr;
	if (m_mainMaterial != nullptr)
	{
		parent = m_mainMaterial;
	}

	// m_instanceList �̃T�C�Y�����낦��
	int subCount = GetCount();
	if (m_instanceList.GetCount() != subCount)
	{
		m_instanceList.Resize(subCount);
	}

	// m_instanceList �̓��e������Ă���
	if (subCount > 0)
	{
		for (int i = 0; i < subCount; ++i)
		{
			m_instanceList[i].Combine(GetAt(i), parent);
		}
	}
	else if (parent != nullptr)
	{
		// parent �͂��邯�� SubMaterial ��1�������Bparent ���g���B
		if (m_instanceList.GetCount() != 1) m_instanceList.Resize(1);
		m_instanceList[0].Combine(parent, nullptr);
	}
	else
	{
		// parent �� SubMaterial �������B�f�t�H���g�̂��g���B
		if (m_instanceList.GetCount() != 1) m_instanceList.Resize(1);
		LN_NOTIMPLEMENTED();
		//m_instanceList[0].Combine(parent, nullptr);
	}
}

LN_NAMESPACE_END