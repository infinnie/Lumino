﻿
#include "Internal.h"
#include <unordered_map>
#include <Lumino/IO/FileManager.h>
#include <Lumino/Graphics/GraphicsException.h>
#include <Lumino/Graphics/Shader.h>
#include <Lumino/Graphics/GraphicsContext.h>
#include "GraphicsManager.h"
#include <Lumino/Graphics/Texture.h>
#include <Lumino/Graphics/Material.h>
#include <Lumino/Rendering/Rendering.h>	// TODO: for LightInfo
#include "CoreGraphicsRenderFeature.h"
#include "RenderingCommand.h"
#include "ShaderAnalyzer.h"

#define LN_CALL_SHADER_COMMAND(func, command, ...) \
	if (m_owner->getManager()->getRenderingType() == GraphicsRenderingType::Threaded) { \
		m_owner->getManager()->getRenderer()->m_primaryCommandList->addCommand<command>(m_deviceObj, __VA_ARGS__); \
	} \
	else { \
		m_deviceObj->func(__VA_ARGS__); \
	}

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GRAPHICS_BEGIN

namespace detail {

//==============================================================================
// ShaderSemanticsManager
//		参考:
//		Unity Builtin Shader variables
//		https://docs.unity3d.com/Manual/SL-UnityShaderVariables.html
//==============================================================================

static std::unordered_map<String, BuiltinSemantics> g_builtinNameMap_CameraUnit =
{
	{ _T("ln_View"), BuiltinSemantics::View },
	{ _T("ln_Projection"), BuiltinSemantics::Projection },
	{ _T("ln_ViewportPixelSize"), BuiltinSemantics::ViewportPixelSize },
};

static std::unordered_map<String, BuiltinSemantics> g_builtinNameMap_ElementUnit =
{
	{ _T("ln_WorldViewProjection"), BuiltinSemantics::WorldViewProjection },
	{ _T("ln_World"), BuiltinSemantics::World },
	{ _T("ln_LightEnables"), BuiltinSemantics::LightEnables },
	{ _T("ln_LightWVPMatrices"), BuiltinSemantics::LightWVPMatrices },
	{ _T("ln_LightDirections"), BuiltinSemantics::LightDirections },
	{ _T("ln_LightPositions"), BuiltinSemantics::LightPositions },
	{ _T("ln_LightZFars"), BuiltinSemantics::LightZFars },
	{ _T("ln_LightDiffuses"), BuiltinSemantics::LightDiffuses },
	{ _T("ln_LightAmbients"), BuiltinSemantics::LightAmbients },
	{ _T("ln_LightSpeculars"), BuiltinSemantics::LightSpeculars },
};

static std::unordered_map<String, BuiltinSemantics> g_builtinNameMap_SubsetUnit =
{
	{ _T("ln_MaterialTexture"), BuiltinSemantics::MaterialTexture },
	{ _T("ln_MaterialDiffuse"), BuiltinSemantics::MaterialDiffuse },
	{ _T("ln_MaterialAmbient"), BuiltinSemantics::MaterialAmbient },
	{ _T("ln_MaterialEmmisive"), BuiltinSemantics::MaterialEmmisive },
	{ _T("ln_MaterialSpecular"), BuiltinSemantics::MaterialSpecular },
	{ _T("ln_MaterialSpecularPower"), BuiltinSemantics::MaterialSpecularPower },
	{ _T("ln_ColorScale"), BuiltinSemantics::ColorScale },
	{ _T("ln_BlendColor"), BuiltinSemantics::BlendColor },
	{ _T("ln_ToneColor"), BuiltinSemantics::ToneColor },
};

//------------------------------------------------------------------------------
ShaderSemanticsManager::ShaderSemanticsManager()
	: m_manager(nullptr)
	, m_sceneVariables()
	, m_cameraVariables()
	, m_elementVariables()
	, m_subsetVariables()
	, m_lastCameraInfoId(0)
	, m_tempBufferWriter(&m_tempBuffer)
{
}

//------------------------------------------------------------------------------
void ShaderSemanticsManager::initialize(GraphicsManager* manager)
{
	if (LN_CHECK_ARG(manager != nullptr)) return;
	m_manager = manager;
}

//------------------------------------------------------------------------------
void ShaderSemanticsManager::tryPushVariable(ShaderVariable* var)
{
	const String& name = var->getName();

	// Camera unit
	{
		auto itr = g_builtinNameMap_CameraUnit.find(name);
		if (itr != g_builtinNameMap_CameraUnit.end())
		{
			m_cameraVariables.add({ var, itr->second });
			return;
		}
	}

	// Element unit
	{
		auto itr = g_builtinNameMap_ElementUnit.find(name);
		if (itr != g_builtinNameMap_ElementUnit.end())
		{
			m_elementVariables.add({ var, itr->second });
			return;
		}
	}

	// Subset unit
	{
		auto itr = g_builtinNameMap_SubsetUnit.find(name);
		if (itr != g_builtinNameMap_SubsetUnit.end())
		{
			m_subsetVariables.add({ var, itr->second });
			return;
		}
	}
}

//------------------------------------------------------------------------------
void ShaderSemanticsManager::updateSceneVariables(const SceneInfo& info)
{

}

//------------------------------------------------------------------------------
void ShaderSemanticsManager::updateCameraVariables(const CameraInfo& info)
{
	if (m_lastCameraInfoId != info.dataSourceId)
	{
		m_lastCameraInfoId = info.dataSourceId;

		for (const VariableKindPair& varInfo : m_cameraVariables)
		{
			switch (varInfo.kind)
			{
				case BuiltinSemantics::View:
					varInfo.variable->setMatrix(info.viewMatrix);
					break;
				case BuiltinSemantics::Projection:
					varInfo.variable->setMatrix(info.projMatrix);
					break;
				case BuiltinSemantics::ViewportPixelSize:
					varInfo.variable->setVector(Vector4(info.viewPixelSize.width, info.viewPixelSize.height, 0, 0));
					break;
				default:
					break;
			}
		}
	}
}

//------------------------------------------------------------------------------
void ShaderSemanticsManager::updateElementVariables(const ElementInfo& info)
{
	DynamicLightInfo** lights = info.affectedLights;

	for (const VariableKindPair& varInfo : m_elementVariables)
	{
		switch (varInfo.kind)
		{
			case BuiltinSemantics::WorldViewProjection:
				varInfo.variable->setMatrix(info.WorldViewProjectionMatrix);
				break;
			case BuiltinSemantics::World:
				varInfo.variable->setMatrix(info.WorldMatrix);
				break;


			// TODO: 以下、ライト列挙時に確定したい。何回もこんな計算するのはちょっと・・
			case BuiltinSemantics::LightEnables:
				m_tempBufferWriter.seek(0, SeekOrigin_Begin);
				for (int i = 0; i < DynamicLightInfo::MaxLights; i++)
				{
					// TODO: WriteBool() がほしい
					m_tempBufferWriter.writeUInt8((lights == nullptr) ? false : lights[i] != nullptr);
				}
				varInfo.variable->setBoolArray((const bool*)m_tempBuffer.getBuffer(), DynamicLightInfo::MaxLights);
				break;
			case BuiltinSemantics::LightWVPMatrices:
				LN_NOTIMPLEMENTED();
				//if (lights != nullptr)
				//{
				//	m_tempBufferWriter.Seek(0, SeekOrigin_Begin);
				//	for (int i = 0; i < DynamicLightInfo::MaxLights; i++)
				//	{
				//		Matrix m = (lights[i] != nullptr) ? (lights[i]->transform * (*info.viewProjMatrix)) : Matrix::Identity;
				//		m_tempBufferWriter.Write(&m, sizeof(Matrix));
				//	}
				//	varInfo.variable->setMatrixArray((const Matrix*)m_tempBuffer.getBuffer(), DynamicLightInfo::MaxLights);
				//}
				break;
			case BuiltinSemantics::LightDirections:
				if (lights != nullptr)
				{
					m_tempBufferWriter.seek(0, SeekOrigin_Begin);
					for (int i = 0; i < DynamicLightInfo::MaxLights; i++)
					{
						// TODO: Vector4::Zero がほしい
						Vector4 v = (lights[i] != nullptr) ? Vector4(lights[i]->m_direction, 0) : Vector4(0, 0, 0, 0);
						m_tempBufferWriter.write(&v, sizeof(Vector4));
					}
					varInfo.variable->setVectorArray((const Vector4*)m_tempBuffer.getBuffer(), DynamicLightInfo::MaxLights);
				}
				break;
			case BuiltinSemantics::LightPositions:
				if (lights != nullptr)
				{
					for (int i = 0; i < DynamicLightInfo::MaxLights; i++)
					{
						// TODO: Vector4::Zero がほしい
						Vector4 v = (lights[i] != nullptr) ? Vector4(lights[i]->transform.getPosition(), 0) : Vector4(0, 0, 0, 0);
						m_tempBufferWriter.write(&v, sizeof(Vector4));
					}
					varInfo.variable->setVectorArray((const Vector4*)m_tempBuffer.getBuffer(), DynamicLightInfo::MaxLights);
				}
				break;
			case BuiltinSemantics::LightZFars:
				if (lights != nullptr)
				{
					m_tempBufferWriter.seek(0, SeekOrigin_Begin);
					for (int i = 0; i < DynamicLightInfo::MaxLights; i++)
					{
						m_tempBufferWriter.writeFloat((lights[i] != nullptr) ? lights[i]->m_shadowZFar : 0.0f);
					}
					varInfo.variable->setFloatArray((const float*)m_tempBuffer.getBuffer(), DynamicLightInfo::MaxLights);
				}
				break;
			case BuiltinSemantics::LightDiffuses:
				if (lights != nullptr)
				{
					m_tempBufferWriter.seek(0, SeekOrigin_Begin);
					for (int i = 0; i < DynamicLightInfo::MaxLights; i++)
					{
						auto& v = (lights[i] != nullptr) ? lights[i]->m_diffuse : Color::Black;
						m_tempBufferWriter.write(&v, sizeof(Color));
					}
					varInfo.variable->setVectorArray((const Vector4*)m_tempBuffer.getBuffer(), DynamicLightInfo::MaxLights);
				}
				break;
			case BuiltinSemantics::LightAmbients:
				if (lights != nullptr)
				{
					m_tempBufferWriter.seek(0, SeekOrigin_Begin);
					for (int i = 0; i < DynamicLightInfo::MaxLights; i++)
					{
						auto& v = (lights[i] != nullptr) ? lights[i]->m_ambient : Color::Transparency;		// TODO: デフォルト値は？
						m_tempBufferWriter.write(&v, sizeof(Color));
					}
					varInfo.variable->setVectorArray((const Vector4*)m_tempBuffer.getBuffer(), DynamicLightInfo::MaxLights);
				}
				break;
			case BuiltinSemantics::LightSpeculars:
				if (lights != nullptr)
				{
					m_tempBufferWriter.seek(0, SeekOrigin_Begin);
					for (int i = 0; i < DynamicLightInfo::MaxLights; i++)
					{
						auto& v = (lights[i] != nullptr) ? lights[i]->m_specular : Color::Black;		// TODO: デフォルト値は？
						m_tempBufferWriter.write(&v, sizeof(Color));
					}
					varInfo.variable->setVectorArray((const Vector4*)m_tempBuffer.getBuffer(), DynamicLightInfo::MaxLights);
				}
				break;

			default:
				break;
		}
	}
}

//------------------------------------------------------------------------------
void ShaderSemanticsManager::updateSubsetVariables(const SubsetInfo& info)
{
	const detail::CombinedMaterial* cm = info.combinedMaterial;

	// TODO: このあたり、もし最適化で変数が消えているなら set しなくて良いようにしたい。

	for (const VariableKindPair& varInfo : m_subsetVariables)
	{
		switch (varInfo.kind)
		{
			case BuiltinSemantics::MaterialTexture:
				varInfo.variable->setTexture((info.materialTexture != nullptr) ? info.materialTexture : m_manager->getDummyWhiteTexture());
				break;
			case BuiltinSemantics::MaterialDiffuse:
				if (cm != nullptr)
					varInfo.variable->setVector(cm->m_diffuse);
				break;
			case BuiltinSemantics::MaterialAmbient:
				if (cm != nullptr)
					varInfo.variable->setVector(cm->m_ambient);
				break;
			case BuiltinSemantics::MaterialEmmisive:
				if (cm != nullptr)
					varInfo.variable->setVector(cm->m_emissive);
				break;
			case BuiltinSemantics::MaterialSpecular:
				if (cm != nullptr)
					varInfo.variable->setVector(cm->m_specular);
				break;
			case BuiltinSemantics::MaterialSpecularPower:
				if (cm != nullptr)
					varInfo.variable->setFloat(cm->m_power);
				break;
			case BuiltinSemantics::ColorScale:
				if (cm != nullptr)
					varInfo.variable->setVector(cm->m_colorScale);
				break;
			case BuiltinSemantics::BlendColor:
				if (cm != nullptr)
					varInfo.variable->setVector(cm->m_blendColor);
				break;
			case BuiltinSemantics::ToneColor:
				if (cm != nullptr)
					varInfo.variable->setVector(cm->m_tone);
				break;
			default:
				break;
		}
	}
}

//------------------------------------------------------------------------------
//void ShaderSemanticsManager::setMaterialTexture(Texture* texture)
//{
//	if (m_subsetVariables[SubsetSemantics::MaterialTexture] != nullptr)
//	{
//		m_subsetVariables[SubsetSemantics::MaterialTexture]->setTexture((texture != nullptr) ? texture : m_manager->getDummyWhiteTexture());
//	}
//}

} // namespace detail


//==============================================================================
// Shader
//==============================================================================

//------------------------------------------------------------------------------
ShaderPtr Shader::getBuiltinShader(BuiltinShader shader)
{
	return detail::GraphicsManager::getInstance()->getBuiltinShader(shader);
}

//------------------------------------------------------------------------------
RefPtr<Shader> Shader::create(const StringRef& filePath, bool useTRSS)
{
	RefPtr<Shader> obj(LN_NEW Shader(), false);
	obj->initialize(detail::GraphicsManager::getInstance(), filePath, useTRSS);
	return obj;
}

//------------------------------------------------------------------------------
RefPtr<Shader> Shader::create(const char* code, int length)
{
	RefPtr<Shader> obj(LN_NEW Shader(), false);
	obj->initialize(detail::GraphicsManager::getInstance(), code, length);
	return obj;
}
//
////------------------------------------------------------------------------------
////
////------------------------------------------------------------------------------
//Shader* Shader::create(GraphicsManager* manager, const void* textData, size_t byteCount)
//{
//	LN_THROW(manager != NULL, ArgumentException);
//	ShaderCompileResult result;
//	RefPtr<Driver::IShader> deviceObj(
//		manager->getGraphicsDevice()->createShader(textData, byteCount, &result), false);
//	LN_THROW(!deviceObj.IsNull(), CompilationException, result);
//	return LN_NEW Shader(manager, deviceObj, ByteBuffer(textData, byteCount));
//}
//
////------------------------------------------------------------------------------
////
////------------------------------------------------------------------------------
//bool Shader::TryCreate(GraphicsManager* manager, const void* textData, size_t byteCount, Shader** outShader, ShaderCompileResult* outResult)
//{
//	LN_THROW(manager != NULL, ArgumentException);
//	LN_THROW(outShader != NULL, ArgumentException);
//
//	*outShader = NULL;
//	RefPtr<Driver::IShader> deviceObj(
//		manager->getGraphicsDevice()->createShader(textData, byteCount, outResult), false);
//	if (deviceObj.IsNull()) {
//		return false;
//	}
//
//	*outShader = LN_NEW Shader(manager, deviceObj, ByteBuffer(textData, byteCount));
//	return true;
//}

//------------------------------------------------------------------------------
Shader::Shader()
	: m_deviceObj(nullptr)
	, m_sourceCode()
	, m_modifiedVariables(true)
{
}

//------------------------------------------------------------------------------
Shader::~Shader()
{
}

//------------------------------------------------------------------------------
void Shader::initialize(detail::GraphicsManager* manager, const StringRef& filePath, bool useTRSS)
{
	RefPtr<Stream> stream(manager->getFileManager()->createFileStream(filePath), false);
	ByteBuffer buf((size_t)stream->getLength() + 1, false);
	stream->read(buf.getData(), buf.getSize());
	buf[(size_t)stream->getLength()] = 0x00;

	initialize(manager, buf.getConstData(), buf.getSize(), useTRSS);

	//GraphicsResourceObject::initialize(manager);
	//

	//

	//ShaderCompileResult result;
	//m_deviceObj = m_manager->getGraphicsDevice()->createShader(buf.GetConstData(), buf.GetSize(), &result);
	//LN_THROW(m_deviceObj != nullptr, CompilationException, result);

	//postInitialize();
}

//------------------------------------------------------------------------------
void Shader::initialize(detail::GraphicsManager* manager, const void* code, int length, bool useTRSS)
{
	GraphicsResourceObject::initialize();

	StringBuilderA newCode;
	if (useTRSS)
	{
		detail::ShaderAnalyzer analyzer;
		analyzer.analyzeLNFX((const char*)code, length);
		auto cc = analyzer.makeHLSLCode();

		newCode.append(cc.data(), cc.size());

		//FileSystem::WriteAllBytes(_T("code.c"), cc.data(), cc.size());
	}
	else
	{
		// ヘッダコード先頭に追加する
		newCode.append(manager->getCommonShaderHeader().c_str());
		newCode.append("#line 5");
		newCode.append(StringA::getNewLine().c_str());
		newCode.append((const char*)code, length);
		newCode.append("\n");	// 最後には改行を入れておく。環境によっては改行がないとエラーになる。しかもエラーなのにエラー文字列が出ないこともある。
	}

	ShaderCompileResult result;
	m_deviceObj = m_manager->getGraphicsDevice()->createShader(newCode.c_str(), newCode.getLength(), &result);
	LN_THROW(m_deviceObj != nullptr, CompilationException, result);

	// ライブラリ外部からの DeviceContext 再設定に備えてコードを保存する
	m_sourceCode.alloc(newCode.c_str(), newCode.getLength());

	postInitialize();
}

//------------------------------------------------------------------------------
void Shader::Dispose()
{
	for (ShaderVariable* var : m_variables)
	{
		LN_SAFE_RELEASE(var);
	}
	m_variables.clear();

	for (ShaderTechnique* tech : m_techniques) 
	{
		LN_SAFE_RELEASE(tech);
	}
	m_techniques.clear();

	LN_SAFE_RELEASE(m_deviceObj);

	GraphicsResourceObject::Dispose();
}

//------------------------------------------------------------------------------
void Shader::postInitialize()
{
	m_semanticsManager.initialize(m_manager);

	// 変数を展開
	for (int i = 0; i < m_deviceObj->getVariableCount(); ++i)
	{
		ShaderVariable* v = LN_NEW ShaderVariable(this, m_deviceObj->getVariable(i));
		m_variables.add(v);
		m_semanticsManager.tryPushVariable(v);
	}

	// テクニックを展開
	for (int i = 0; i < m_deviceObj->getTechniqueCount(); ++i)
	{
		m_techniques.add(LN_NEW ShaderTechnique(this, m_deviceObj->getTechnique(i)));
	}
}

//------------------------------------------------------------------------------
void Shader::tryCommitChanges()
{
	auto* serializer = getManager()->getShaderVariableCommitSerializeHelper();
	serializer->beginSerialize();
	for (ShaderVariable* v : getVariables())
	{
		v->onCommitChanges();
		serializer->writeValue(v->getDeviceObject(), v->m_value);
	}

	detail::RenderBulkData varsData(serializer->getSerializeData(), serializer->getSerializeDataLength());

	auto* cmdList = getManager()->getPrimaryRenderingCommandList();
	byte_t* data = (byte_t*)varsData.alloc(cmdList);

	detail::GraphicsManager* manager = getManager();
	Shader* _this = this;
	LN_ENQUEUE_RENDER_COMMAND_3(
		CommitChanges, getManager(),
		detail::GraphicsManager*, manager,
		detail::RenderBulkData, varsData,
		RefPtr<Shader>, _this,
		{
			auto* serializer = manager->getShaderVariableCommitSerializeHelper();
			serializer->deserialize(varsData.getData(), varsData.getSize());
		});
}

//------------------------------------------------------------------------------
ShaderVariable* Shader::findVariable(const TCHAR* name, CaseSensitivity cs) const
{
	for (ShaderVariable* var : m_variables) {
		if (var->getName().compare(name, -1, cs) == 0) {
			return var;
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
const List<ShaderTechnique*>& Shader::getTechniques() const
{
	return m_techniques;
}

//------------------------------------------------------------------------------
ShaderTechnique* Shader::findTechnique(const TCHAR* name, CaseSensitivity cs) const
{
	for (auto* var : m_techniques) {
		if (var->getName().compare(name, -1, cs) == 0) {
			return var;
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
void Shader::onChangeDevice(Driver::IGraphicsDevice* device)
{
	if (device == NULL)
	{
		LN_SAFE_RELEASE(m_deviceObj);
	}
	else
	{
		ShaderCompileResult result;
		m_deviceObj = m_manager->getGraphicsDevice()->createShader(m_sourceCode.getConstData(), m_sourceCode.getSize(), &result);
		LN_THROW(result.Level != ShaderCompileResultLevel_Error, InvalidOperationException);	// 一度生成に成功しているので発生はしないはず

		// 変数再割り当て
		int varCount = m_deviceObj->getVariableCount();
		for (int i = 0; i < varCount; ++i)
		{
			auto* varObj = m_deviceObj->getVariable(i);
			auto* var = findVariable(varObj->getName());
			var->changeDevice(varObj);
		}

		// テクニック再割り当て
		int techCount = m_deviceObj->getTechniqueCount();
		for (int i = 0; i < techCount; ++i)
		{
			auto* techObj = m_deviceObj->getTechnique(i);
			auto* tech = findTechnique(techObj->getName());
			tech->changeDevice(techObj);
		}
	}
}

//------------------------------------------------------------------------------
void Shader::setShaderValue(uint32_t variableNameHash, const ShaderValue& value)
{
	for (ShaderVariable* v : m_variables)
	{
		if (v->getNameHash() == variableNameHash)
		{
			v->setShaderValue(value);
			return;
		}
	}
}


//==============================================================================
// ShaderValue
//==============================================================================

//------------------------------------------------------------------------------
ShaderValue::ShaderValue()
	: m_hashCode(0)
	, m_hashDirty(true)
{
	m_type = ShaderVariableType_Unknown;
	memset(&m_value, 0, sizeof(m_value));
}

//------------------------------------------------------------------------------
ShaderValue::~ShaderValue()
{
	releaseValueBuffer();
}

//------------------------------------------------------------------------------
ShaderValue::ShaderValue(const ShaderValue& obj)
{
	memset(&m_value, 0, sizeof(m_value));
	copy(obj);
}
ShaderValue& ShaderValue::operator = (const ShaderValue& obj)
{
	memset(&m_value, 0, sizeof(m_value));
	copy(obj);
	return (*this);
}

//------------------------------------------------------------------------------
void ShaderValue::setBool(bool value)
{
	m_type = ShaderVariableType_Bool;
	m_value.BoolVal = value;
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void ShaderValue::setInt(int value)
{
	m_type = ShaderVariableType_Int;
	m_value.Int = value;
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void ShaderValue::setBoolArray(const bool* values, int count)
{
	m_type = ShaderVariableType_BoolArray;
	allocValueBuffer(sizeof(bool) * count);
	if (values != NULL) {
		memcpy(m_value.BoolArray, values, sizeof(bool) * count);
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void ShaderValue::setFloat(float value)
{
	m_type = ShaderVariableType_Float;
	m_value.Float = value;
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void ShaderValue::setFloatArray(const float* values, int count)
{
	m_type = ShaderVariableType_FloatArray;
	allocValueBuffer(sizeof(float) * count);
	if (values != NULL) {
		memcpy(m_value.FloatArray, values, sizeof(float) * count);
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void ShaderValue::setVector(const Vector4& vec)
{
	m_type = ShaderVariableType_Vector;
	allocValueBuffer(sizeof(Vector4));
	*m_value.Vector = vec;
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void ShaderValue::setVectorArray(const Vector4* vectors, int count)
{
	m_type = ShaderVariableType_VectorArray;
	allocValueBuffer(sizeof(Vector4) * count);
	if (vectors != NULL) {
		memcpy(m_value.VectorArray, vectors, sizeof(Vector4) * count);
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void ShaderValue::setMatrix(const Matrix& matrix)
{
	m_type = ShaderVariableType_Matrix;
	allocValueBuffer(sizeof(Matrix));
	*m_value.Matrix = matrix;
	m_hashDirty = true;
}


//------------------------------------------------------------------------------
void ShaderValue::setMatrixArray(const Matrix* matrices, int count)
{
	m_type = ShaderVariableType_MatrixArray;
	allocValueBuffer(sizeof(Matrix) * count);
	if (matrices != NULL) {
		memcpy(m_value.MatrixArray, matrices, sizeof(Matrix) * count);
		m_hashDirty = true;
	}
}

//------------------------------------------------------------------------------
void ShaderValue::setDeviceTexture(Driver::ITexture* texture)
{
	m_type = ShaderVariableType_DeviceTexture;
	LN_REFOBJ_SET(m_value.DeviceTexture, texture);
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void ShaderValue::setManagedTexture(Texture* texture)
{
	m_type = ShaderVariableType_ManagedTexture;
	LN_REFOBJ_SET(m_value.ManagedTexture, texture);
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void ShaderValue::setString(const char* str)
{
	m_type = ShaderVariableType_String;
	String s;
	s.assignCStr(str);
	size_t size = s.getByteCount() + sizeof(TCHAR);
	allocValueBuffer(size);
	memcpy(m_value.String, s.c_str(), size);
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
void ShaderValue::setString(const String& s)
{
	m_type = ShaderVariableType_String;
	size_t size = s.getByteCount() + sizeof(TCHAR);
	allocValueBuffer(size);
	memcpy(m_value.String, s.c_str(), size);
	m_hashDirty = true;
}

//------------------------------------------------------------------------------
bool ShaderValue::equals(const ShaderValue& value) const
{
	if (m_type != value.m_type) return false;

	switch (m_type)
	{
		case ShaderVariableType_Bool:
			return m_value.BoolVal == value.m_value.BoolVal;
		case ShaderVariableType_BoolArray:
			return m_buffer.equals(value.m_buffer);
		case ShaderVariableType_Int:
			return m_value.Int == value.m_value.Int;
		case ShaderVariableType_Float:
			return m_value.Float == value.m_value.Float;
		case ShaderVariableType_FloatArray:
			return m_buffer.equals(value.m_buffer);
		case ShaderVariableType_Vector:
			return m_value.Vector == value.m_value.Vector;
		case ShaderVariableType_VectorArray:
			return m_buffer.equals(value.m_buffer);
		case ShaderVariableType_Matrix:
			return m_value.Matrix == value.m_value.Matrix;
		case ShaderVariableType_MatrixArray:
			return m_buffer.equals(value.m_buffer);
		case ShaderVariableType_DeviceTexture:
			return m_value.DeviceTexture == value.m_value.DeviceTexture;
		case ShaderVariableType_ManagedTexture:
			return m_value.ManagedTexture == value.m_value.ManagedTexture;
		case ShaderVariableType_String:
			return StringTraits::compare(m_value.String, m_value.String, -1) == 0;
		default:
			assert(0);
			break;
	}
	return false;
}

//------------------------------------------------------------------------------
uint32_t ShaderValue::getHashCode()
{
	if (m_hashDirty)
	{
		m_hashCode = Hash::calcHash(reinterpret_cast<const char*>(&m_value), sizeof(m_value));
		m_hashCode += (int)m_type;

		if (IsBufferCopyType(m_type))
		{
			m_hashCode += Hash::calcHash((const char*)m_buffer.getConstData(), m_buffer.getSize());
		}

		m_hashDirty = false;
	}

	return m_hashCode;
}

//------------------------------------------------------------------------------
int ShaderValue::getArrayLength() const
{
	if (m_type == ShaderVariableType_BoolArray) {
		return m_buffer.getSize() / sizeof(bool);
	}
	if (m_type == ShaderVariableType_FloatArray) {
		return m_buffer.getSize() / sizeof(float);
	}
	if (m_type == ShaderVariableType_VectorArray) {
		return m_buffer.getSize() / sizeof(Vector4);
	}
	if (m_type == ShaderVariableType_MatrixArray) {
		return m_buffer.getSize() / sizeof(Matrix);
	}
	return 0;
}

//------------------------------------------------------------------------------
void ShaderValue::releaseValueBuffer()
{
	/*if (m_value.ByteCount > 0) {
		LN_SAFE_DELETE_ARRAY(m_value.Buffer);
	}
	else */
	if (m_type == ShaderVariableType_DeviceTexture) {
		LN_SAFE_RELEASE(m_value.DeviceTexture);
	}
	if (m_type == ShaderVariableType_ManagedTexture) {
		LN_SAFE_RELEASE(m_value.ManagedTexture);
	}
	//m_value.ByteCount = 0;
	m_buffer.free();
}

//------------------------------------------------------------------------------
void ShaderValue::allocValueBuffer(size_t byteCount)
{
	if (byteCount > m_buffer.getSize()/*m_value.ByteCount*//* || m_buffer.GetReferenceCount() != 1*/)
	{
		//LN_SAFE_DELETE_ARRAY(m_value.Buffer);
		//m_value.Buffer = LN_NEW byte_t[byteCount];
		//m_value.ByteCount = byteCount;
		//m_buffer.Attach(LN_NEW ByteBuffer(byteCount), false);
		m_buffer.resize(byteCount);
		m_value.Buffer = m_buffer.getData();
	}
}

//------------------------------------------------------------------------------
void ShaderValue::copy(const ShaderValue& value)
{
	m_type = value.m_type;
	m_buffer = value.m_buffer;	// 共有参照
	m_hashCode = value.m_hashCode;
	m_hashDirty = value.m_hashDirty;

	//if (m_buffer.IsNull()) {
	//	memcpy(&m_value, &value.m_value, sizeof(m_value));
	//}
	//else {
	//	m_value.Buffer = m_buffer->GetData();
	//}
	if (IsBufferCopyType(m_type))
	{
		m_value.Buffer = m_buffer.getData();
	}
	else
	{
		m_value = value.m_value;
	}

	if (m_type == ShaderVariableType_DeviceTexture) {
		m_value.DeviceTexture = value.m_value.DeviceTexture;
		LN_SAFE_ADDREF(m_value.DeviceTexture);
	}
	if (m_type == ShaderVariableType_ManagedTexture) {
		m_value.ManagedTexture = value.m_value.ManagedTexture;
		LN_SAFE_ADDREF(m_value.ManagedTexture);
	}

}

//==============================================================================
// ShaderVariable
//==============================================================================

//------------------------------------------------------------------------------
ShaderVariable::ShaderVariable(Shader* owner, Driver::IShaderVariable* deviceObj)
	: m_owner(owner)
	, m_deviceObj(deviceObj)
	, m_textureValue(NULL)
	, m_modified(true)
{
	m_name = deviceObj->getName();
	m_nameHash = Hash::calcHash(m_name.c_str(), m_name.getLength());

	// 初期値として保持しておく
	m_value = deviceObj->getValue();

	// アノテーションの展開
	for (int i = 0; i < m_deviceObj->getAnnotationCount(); ++i) {
		m_annotations.add(LN_NEW ShaderVariable(m_owner, m_deviceObj->getAnnotation(i)));
	}
}

//------------------------------------------------------------------------------
ShaderVariable::~ShaderVariable()
{
	for (ShaderVariable* anno : m_annotations) {
		anno->release();
	}
	LN_SAFE_RELEASE(m_textureValue);
}

//------------------------------------------------------------------------------
ShaderVariableType ShaderVariable::getType() const
{
	return m_deviceObj->getType();
}

//------------------------------------------------------------------------------
const String& ShaderVariable::getName() const
{
	return m_name;
}

//------------------------------------------------------------------------------
const String& ShaderVariable::getSemanticName() const
{
	return m_deviceObj->getSemanticName();
}

//------------------------------------------------------------------------------
int ShaderVariable::getRows() const
{
	return m_deviceObj->getMatrixRows();
}

//------------------------------------------------------------------------------
int ShaderVariable::getColumns() const
{
	return m_deviceObj->getMatrixColumns();
}

//------------------------------------------------------------------------------
int ShaderVariable::getArrayElements() const
{
	return m_deviceObj->getArrayElements();
}

//------------------------------------------------------------------------------
void ShaderVariable::setBool(bool value)
{
	if (m_value.getType() != ShaderVariableType_Bool || value != m_value.getBool())
	{
		setModified();
		m_value.setBool(value);
	}
}

//------------------------------------------------------------------------------
bool ShaderVariable::getBool() const
{
	return m_value.getBool();
}

//------------------------------------------------------------------------------
void ShaderVariable::setInt(int value)
{
	if (m_value.getType() != ShaderVariableType_Int || value != m_value.getInt())
	{
		setModified();
		m_value.setInt(value);
	}
}

//------------------------------------------------------------------------------
void ShaderVariable::setBoolArray(const bool* values, int count)
{
	// TODO: != チェックした方がパフォーマンス良い？
	setModified();
	m_value.setBoolArray(values, count);
}

//------------------------------------------------------------------------------
int ShaderVariable::getInt() const
{
	return m_value.getInt();
}

//------------------------------------------------------------------------------
void ShaderVariable::setFloat(float value)
{
	if (m_value.getType() != ShaderVariableType_Float || value != m_value.getFloat())
	{
		setModified();
		m_value.setFloat(value);
	}
}

//------------------------------------------------------------------------------
float ShaderVariable::getFloat() const
{
	return m_value.getFloat();
}

//------------------------------------------------------------------------------
void ShaderVariable::setFloatArray(const float* values, int count)
{
	// TODO: != チェックした方がパフォーマンス良い？
	setModified();
	m_value.setFloatArray(values, count);
}

//------------------------------------------------------------------------------
void ShaderVariable::setVector(const Vector4& value)
{
	if (m_value.getType() != ShaderVariableType_Vector || value != m_value.getVector())
	{
		setModified();
		m_value.setVector(value);
	}
}

//------------------------------------------------------------------------------
const Vector4& ShaderVariable::getVector() const
{
	return m_value.getVector();
}

//------------------------------------------------------------------------------
void ShaderVariable::setVectorArray(const Vector4* values, int count)
{
	// TODO: != チェックした方がパフォーマンス良い？
	setModified();
	m_value.setVectorArray(values, count);
}

//------------------------------------------------------------------------------
const Vector4* ShaderVariable::getVectorArray() const
{
	return m_value.getVectorArray();
}

//------------------------------------------------------------------------------
void ShaderVariable::setMatrix(const Matrix& value)
{
	if (m_value.getType() != ShaderVariableType_Matrix || value != m_value.getMatrix())
	{
		setModified();
		m_value.setMatrix(value);
	}
}

//------------------------------------------------------------------------------
const Matrix& ShaderVariable::getMatrix() const
{
	return m_value.getMatrix();
}

//------------------------------------------------------------------------------
void ShaderVariable::setMatrixArray(const Matrix* values, int count)
{
	// TODO: != チェックした方がパフォーマンス良い？
	setModified();
	m_value.setMatrixArray(values, count);
}

//------------------------------------------------------------------------------
const Matrix* ShaderVariable::getMatrixArray() const
{
	return m_value.getMatrixArray();
}

//------------------------------------------------------------------------------
void ShaderVariable::setTexture(Texture* texture)
{
	//if (m_value.GetType() != ShaderVariableType_ManagedTexture || texture != m_value.getManagedTexture())
	//{
	//	setModified();
	//	m_value.setManagedTexture();
	//}
	bool modified = false;
	if (m_value.getType() == ShaderVariableType_DeviceTexture)
	{
		if (texture == nullptr)
		{
			if (m_value.getDeviceTexture() != nullptr)
			{
				modified = true;
			}
		}
		else if (texture->resolveDeviceObject() != m_value.getDeviceTexture())
		{
			modified = true;
		}
	}
	else
	{
		modified = true;
	}

	if (modified)
	{
		setModified();
		Driver::ITexture* t = (texture != nullptr) ? texture->resolveDeviceObject() : nullptr;
		m_value.setDeviceTexture(t);
		LN_REFOBJ_SET(m_textureValue, texture);
	}
}

//------------------------------------------------------------------------------
Texture* ShaderVariable::getTexture() const
{
	//return m_value.getDeviceTexture();
	return m_textureValue;
}

//------------------------------------------------------------------------------
void ShaderVariable::setString(const char* str)
{
	m_value.setString(str);
}

//------------------------------------------------------------------------------
const TCHAR* ShaderVariable::getString() const
{
	return m_value.getString();
}

//------------------------------------------------------------------------------
void ShaderVariable::setShaderValue(const ShaderValue& value)
{
	if (value.getType() == ShaderVariableType_ManagedTexture)
	{
		// テクスチャ型の場合はちょっと特殊
		setTexture(value.getManagedTexture());
	}
	else
	{
		if (!m_value.equals(value))
		{
			setModified();
			m_value = value;
		}
	}
}

//------------------------------------------------------------------------------
const List<ShaderVariable*>& ShaderVariable::getAnnotations() const
{
	return m_annotations;
}

//------------------------------------------------------------------------------
ShaderVariable* ShaderVariable::findAnnotation(const TCHAR* name, CaseSensitivity cs) const
{
	for (ShaderVariable* anno : m_annotations) {
		if (anno->getName().compare(name, -1, cs) == 0) {
			return anno;
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------
void ShaderVariable::changeDevice(Driver::IShaderVariable* obj)
{
	if (obj == nullptr) {
	}
	else
	{
		m_deviceObj = obj;	// 今は特に参照カウントを操作してはいないのでこれだけ

		// アノテーション再割り当て
		int annoCount = m_deviceObj->getAnnotationCount();
		for (int i = 0; i < annoCount; ++i)
		{
			auto* annoObj = m_deviceObj->getAnnotation(i);
			auto* anno = findAnnotation(annoObj->getName());
			anno->changeDevice(annoObj);
		}
	}
}

//------------------------------------------------------------------------------
void ShaderVariable::setModified()
{
	detail::ContextInterface* activeContext = m_owner->getManager()->getActiveContext();
	if (activeContext != nullptr) activeContext->onShaderVariableModified(this);
	m_owner->setModifiedVariables(true);
	m_modified = true;
}

//------------------------------------------------------------------------------
void ShaderVariable::onCommitChanges()
{
	if (m_modified)
	{
		m_modified = false;
	}

	if (m_textureValue != nullptr)
	{
		m_textureValue->resolveDeviceObject();
	}
}


//==============================================================================
// ShaderTechnique
//==============================================================================

//------------------------------------------------------------------------------
ShaderTechnique::ShaderTechnique(Shader* owner, Driver::IShaderTechnique* deviceObj)
	: m_owner(owner)
	, m_deviceObj(deviceObj)
{
	m_name = m_deviceObj->getName();

	// パスの展開
	for (int i = 0; i < m_deviceObj->getPassCount(); ++i) {
		m_passes.add(LN_NEW ShaderPass(m_owner, m_deviceObj->getPass(i)));
	}

	// アノテーションの展開
	for (int i = 0; i < m_deviceObj->getAnnotationCount(); ++i) {
		m_annotations.add(LN_NEW ShaderVariable(m_owner, m_deviceObj->getAnnotation(i)));
	}
}

//------------------------------------------------------------------------------
ShaderTechnique::~ShaderTechnique()
{
	for (ShaderPass* pass : m_passes) {
		LN_SAFE_RELEASE(pass);
	}
	for (ShaderVariable* anno : m_annotations) {
		LN_SAFE_RELEASE(anno);
	}
}

//------------------------------------------------------------------------------
const String& ShaderTechnique::getName() const
{
	return m_name;
}

//------------------------------------------------------------------------------
const List<ShaderPass*>& ShaderTechnique::getPasses() const
{
	return m_passes;
}

//------------------------------------------------------------------------------
ShaderPass* ShaderTechnique::getPass(const TCHAR* name) const
{
	auto itr = std::find_if(m_passes.begin(), m_passes.end(), [name](ShaderPass* pass) { return pass->getName() == name; });
	LN_THROW(itr != m_passes.end(), KeyNotFoundException);
	return *itr;
}

//------------------------------------------------------------------------------
const List<ShaderVariable*>& ShaderTechnique::getAnnotations() const
{
	return m_annotations;
}

//------------------------------------------------------------------------------
ShaderVariable* ShaderTechnique::findAnnotation(const TCHAR* name, CaseSensitivity cs) const
{
	for (ShaderVariable* anno : m_annotations) {
		if (anno->getName().compare(name, -1, cs) == 0) {
			return anno;
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
void ShaderTechnique::changeDevice(Driver::IShaderTechnique* obj)
{
	if (obj == NULL) {
	}
	else
	{
		m_deviceObj = obj;	// 今は特に参照カウントを操作してはいないのでこれだけ

		// パス再割り当て
		int passCount = m_deviceObj->getPassCount();
		for (int i = 0; i < passCount; ++i)
		{
			auto* passObj = m_deviceObj->getPass(i);
			auto* pass = getPass(passObj->getName());
			pass->changeDevice(passObj);
		}

		// アノテーション再割り当て
		int annoCount = m_deviceObj->getAnnotationCount();
		for (int i = 0; i < annoCount; ++i)
		{
			auto* annoObj = m_deviceObj->getAnnotation(i);
			auto* anno = findAnnotation(annoObj->getName());
			anno->changeDevice(annoObj);
		}
	}
}

//==============================================================================
// ShaderPass
//==============================================================================

//------------------------------------------------------------------------------
ShaderPass::ShaderPass(Shader* owner, Driver::IShaderPass* deviceObj)
	: m_owner(owner)
	, m_deviceObj(deviceObj)
	, m_name(m_deviceObj->getName())
{
	// アノテーションの展開
	for (int i = 0; i < m_deviceObj->getAnnotationCount(); ++i) {
		m_annotations.add(LN_NEW ShaderVariable(m_owner, m_deviceObj->getAnnotation(i)));
	}
}

//------------------------------------------------------------------------------
ShaderPass::~ShaderPass()
{
	for (ShaderVariable* anno : m_annotations) {
		LN_SAFE_RELEASE(anno);
	}
}

//------------------------------------------------------------------------------
const String& ShaderPass::getName() const
{
	return m_name;
}

//------------------------------------------------------------------------------
void ShaderPass::apply()
{
	m_owner->tryCommitChanges();

	//LN_CALL_SHADER_COMMAND(apply, ApplyShaderPassCommand);
    if (m_owner->getManager()->getRenderingType() == GraphicsRenderingType::Threaded) {
        m_owner->getManager()->getRenderer()->m_primaryCommandList->addCommand<ApplyShaderPassCommand>(m_deviceObj);
    }
    else {
		m_owner->getManager()->getGraphicsDevice()->getRenderer()->setShaderPass(m_deviceObj);
    }
}

//------------------------------------------------------------------------------
const List<ShaderVariable*>& ShaderPass::getAnnotations() const
{
	return m_annotations;
}

//------------------------------------------------------------------------------
ShaderVariable* ShaderPass::findAnnotation(const TCHAR* name, CaseSensitivity cs) const
{
	for (ShaderVariable* anno : m_annotations) {
		if (anno->getName().compare(name, -1, cs) == 0) {
			return anno;
		}
	}
	return NULL;
}

//------------------------------------------------------------------------------
void ShaderPass::changeDevice(Driver::IShaderPass* obj)
{
	if (obj == NULL) {
	}
	else
	{
		m_deviceObj = obj;	// 今は特に参照カウントを操作してはいないのでこれだけ

		// アノテーション再割り当て
		int annoCount = m_deviceObj->getAnnotationCount();
		for (int i = 0; i < annoCount; ++i)
		{
			auto* annoObj = m_deviceObj->getAnnotation(i);
			auto* anno = findAnnotation(annoObj->getName());
			anno->changeDevice(annoObj);
		}
	}
}

LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END
