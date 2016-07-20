﻿
#pragma once
#include "../Graphics/Color.h"
#include "../Graphics/Mesh.h"	// TODO:

LN_NAMESPACE_BEGIN

/**
	@brief
*/
//class Material2
//	: public Object
//{
//public:
//	void SetOpacity(float opacity) { m_opacity = opacity; }
//	float GetOpacity() const { return m_opacity; }
//	void SetColorScale(const Color32& color) { m_colorScale = color; }
//	const Color& GetColorScale() const { return m_colorScale; }
//	void SetBlendColor(const Color32& color) { m_blendColor = color; }
//	const Color& GetBlendColor() const { return m_blendColor; }
//	void SetTone(const ToneF& tone) { m_tone = tone; }
//	const ToneF& GetTone() const { return m_tone; }
//	void SetTexture(Texture* texture);
//	Texture* GetTexture() const { return m_texture; }
//
//	const Matrix& GetUVTransform() const { return m_uvTransform; }
//	
//	void SetShader(Shader* shader);
//	Shader* GetShader() const { return m_shader; }
//	
//	void SetIntParameter(const StringRef& name, const int value);
//	void SetFloatParameter(const StringRef& name, int value);
//	void SetVectorParameter(const StringRef& name, const Vector4& value);
//	void SetMatrixParameter(const StringRef& name, const Matrix& value);
//	void SetTextureParameter(const StringRef& name, Texture* value);
//	
//LN_INTERNAL_ACCESS:
//	Material2(int materialTypeId = 0);
//	virtual ~Material2();
//	inline int GetMaterialTypeId() const { return m_materialTypeId; }
//
//private:
//	int			m_materialTypeId;
//	float		m_opacity;
//	Color		m_colorScale;
//	Color		m_blendColor;
//	ToneF		m_tone;
//	Texture*	m_texture;
//	Matrix		m_uvTransform;	// TODO: Unity のように offset,scale で持ったほうがメモリ効率いい。
//	Shader*		m_shader;
//};

namespace detail
{

static const int MmdMaterialTypeId = 12345;

class MaterialInstance
	: public RefObject
{
public:
	Material3*	m_owner;
	Color		m_colorScale;	// 乗算結合済み (opacity 込み)
	Color		m_blendColor;	// 加算結合済み
	ToneF		m_tone;			// 加算結合済み
	Shader*		m_shader;

	void Combine(Material3* owner, Material3* parent);

	virtual void OnCombine(Material3* owner, Material3* parent)
	{
		m_colorScale = m_owner->GetColorScale();
		m_colorScale.a *= m_owner->GetOpacity();
		m_blendColor = m_owner->GetBlendColor();
		m_tone = m_owner->GetTone();
		m_shader = m_owner->GetShader();

		// combine
		if (parent != nullptr)
		{
			m_colorScale.MultiplyClamp(parent->GetColorScale());
			m_colorScale.a *= parent->GetOpacity();
			m_blendColor.AddClamp(parent->GetBlendColor());
			m_tone.AddClamp(parent->GetTone());
			if (m_shader == nullptr) {
				m_shader = parent->GetShader();
			}
		}
	}

	const Matrix& GetUVTransform() const { return m_owner->GetUVTransform(); }

	//void Combine(Material2* owner, MaterialInstance* parent)
	//{
	//	// set
	//	m_owner = owner;
	//	m_colorScale = m_owner->GetColorScale();
	//	m_colorScale.a *= static_cast<float>(m_owner->GetOpacity()) / 255.f;
	//	m_blendColor = m_owner->GetBlendColor();
	//	m_tone = m_owner->GetTone();
	//	m_shader = m_owner->GetShader();
	//	
	//	// combine
	//	m_colorScale.MultiplyClamp(parent->m_colorScale);
	//	m_blendColor.AddClamp(parent->m_blendColor);
	//	m_tone.AddClamp(parent->m_tone);
	//	if (m_shader != nullptr) {
	//		m_shader = parent->m_shader;
	//	}
	//}
};

} // namespace detail

/**
	@brief
*/
class MaterialList2
	: public tr::ReflectionObjectList<Material3*>	// SubMaterials (0 の場合もある)
{
public:
	Material3* GetMainMaterial() const { return m_mainMaterial; }
	
LN_INTERNAL_ACCESS:
	MaterialList2();
	virtual ~MaterialList2();
	void Initialize(int subMaterialCount, bool createMainMaterial);
	void UpdateMaterialInstances();
	int GetMaterialInstanceCount() const { return m_instanceList.GetCount(); }
	detail::MaterialInstance* GetMaterialInstance(int index) { return m_instanceList[index]; }
	
private:
	RefPtr<Material3>				m_mainMaterial;
	Array<RefPtr<detail::MaterialInstance>>	m_instanceList;
};

LN_NAMESPACE_END
