﻿
#include "Internal.hpp"
#include <LuminoEngine/Engine/Diagnostics.hpp>
#include <LuminoEngine/Graphics/Texture.hpp>
#include <LuminoEngine/Graphics/SamplerState.hpp>
#include <LuminoEngine/Graphics/GraphicsContext.hpp>
#include <LuminoEngine/Graphics/RenderPass.hpp>
#include <LuminoEngine/Shader/Shader.hpp>
#include "../Graphics/GraphicsDeviceContext.hpp"
#include "../Graphics/GraphicsManager.hpp"
#include "UnifiedShaderCompiler.hpp"
#include "ShaderManager.hpp"
#include "ShaderTranspiler.hpp"
#include "HLSLMetadataParser.hpp"

namespace ln {

// int or float
static void alignScalarsToBuffer(
    const byte_t* source,
    size_t unitBytes,
    int unitCount,
    byte_t* buffer,
    size_t offset,
    int elements,
    size_t arrayStride) LN_NOEXCEPT
{
    byte_t* head = buffer + offset;
    int loop = std::min(unitCount, elements);
    for (int i = 0; i < loop; i++) {
        memcpy(head + arrayStride * i, source + unitBytes * i, unitBytes);
    }
}

// vector
static void alignVectorsToBuffer(
    const byte_t* source,
    int sourceColumns,
    int sourceElementCount,
    byte_t* buffer,
    size_t offset,
    int elements,
    size_t arrayStride,
    int columns) LN_NOEXCEPT
{
    size_t srcVectorSize = sizeof(float) * sourceColumns;
    size_t copySize = std::min(srcVectorSize, sizeof(float) * columns);
    byte_t* head = buffer + offset;
    int loop = std::min(sourceElementCount, elements);
    for (int i = 0; i < loop; i++) {
        memcpy(head + arrayStride * i, source + srcVectorSize * i, copySize);
    }
}

// matrix
static void alignMatricesToBuffer(
    const byte_t* source,
    int sourceColumns,
    int sourceRows,
    int sourceElementCount,
    byte_t* buffer,
    size_t offset,
    int elements,
    size_t matrixStride,
    size_t arrayStride,
    int rows,
    int columns,
    bool transpose) LN_NOEXCEPT
{
    size_t srcRowSize = sizeof(float) * sourceColumns;
    size_t dstRowSize = matrixStride;
    size_t copySize = std::min(srcRowSize, dstRowSize);
    byte_t* head = buffer + offset;
    int loop = std::min(sourceElementCount, elements);
    int rowLoop = std::min(sourceRows, rows);
    for (int i = 0; i < loop; i++) {
        const byte_t* srcMatHead = source + (sourceColumns * sourceRows * sizeof(float)) * i;
        byte_t* dstMatHead = head + arrayStride * i;

        float tmp[16];
        if (transpose) {
            assert(sourceColumns == 4 && sourceRows == 4);
            *reinterpret_cast<Matrix*>(tmp) = Matrix::makeTranspose(*reinterpret_cast<const Matrix*>(srcMatHead));
            srcMatHead = reinterpret_cast<const byte_t*>(tmp);
        }

        for (int j = 0; j < rowLoop; j++) {
            memcpy(dstMatHead + matrixStride * j, srcMatHead + srcRowSize * j, copySize);
        }
    }
}

//=============================================================================
// ShaderCompilationProperties

ShaderCompilationProperties::ShaderCompilationProperties()
{
}

ShaderCompilationProperties::~ShaderCompilationProperties()
{
}

void ShaderCompilationProperties::init()
{
}

void ShaderCompilationProperties::addIncludeDirectory(const StringRef& value)
{
    m_includeDirectories.add(value);
}

void ShaderCompilationProperties::addDefinition(const StringRef& value)
{
    m_definitions.add(value);
}

void ShaderCompilationProperties::setDiagnostics(DiagnosticsManager* diag)
{
    m_diag = diag;
}

//=============================================================================
// Shader

Ref<Shader> Shader::create(const StringRef& hlslEffectFilePath, ShaderCompilationProperties* properties)
{
    return ln::makeObject<Shader>(hlslEffectFilePath, properties);
}

Ref<Shader> Shader::create(const StringRef& vertexShaderFilePath, const StringRef& pixelShaderFilePath, ShaderCompilationProperties* properties)
{
    return ln::makeObject<Shader>(vertexShaderFilePath, pixelShaderFilePath, properties);
}

Shader::Shader()
    : m_manager(detail::EngineDomain::shaderManager())
    , m_name()
    , m_buffers()
    , m_techniques(makeList<Ref<ShaderTechnique>>())
    , m_textureParameters()
    , m_globalConstantBuffer(nullptr)
    , m_semanticsManager()
{
}

Shader::~Shader()
{
}

void Shader::init()
{
    GraphicsResource::init();
}

void Shader::init(const StringRef& filePath, ShaderCompilationProperties* properties)
{
    Shader::init();
    Ref<DiagnosticsManager> localDiag = nullptr;
    if (properties) localDiag = properties->m_diag;
    if (!localDiag) localDiag = makeObject<DiagnosticsManager>();

    if (Path(filePath).hasExtension(detail::UnifiedShader::FileExt)) {
        auto file = FileStream::create(filePath, FileOpenMode::Read);
        createFromStream(file, localDiag);
    } else {
#ifdef LN_BUILD_EMBEDDED_SHADER_TRANSCOMPILER
		ByteBuffer buffer = FileSystem::readAllBytes(filePath);

		List<Path> includeDirs;
		List<String> definitions;
		if (properties) {
			for (auto& path : properties->m_includeDirectories) includeDirs.add(path);
			for (auto& def : properties->m_definitions) definitions.add(def);
		}

		detail::UnifiedShaderCompiler compiler(m_manager, localDiag);
		if (!compiler.compile(reinterpret_cast<char*>(buffer.data()), buffer.size(), includeDirs, definitions)) {
			LN_ERROR(localDiag->toString());
			return;
		}
		if (!compiler.link()) {
			LN_ERROR();
			return;
		}

		createFromUnifiedShader(compiler.unifiedShader(), localDiag);
#else
        LN_NOTIMPLEMENTED();
#endif
    }

    postInitialize();
    m_name = Path(filePath).fileNameWithoutExtension();

    if (!properties || !properties->m_diag) {
        if (localDiag->hasError()) {
            LN_ERROR(localDiag->toString());
            return;
        } else if (localDiag->hasWarning()) {
            LN_LOG_WARNING << localDiag->toString();
        }
    }
}

void Shader::init(const StringRef& vertexShaderFilePath, const StringRef& pixelShaderFilePath, ShaderCompilationProperties* properties)
{
    Shader::init();
    Ref<DiagnosticsManager> localDiag = nullptr;
    if (properties) localDiag = properties->m_diag;
    if (!localDiag) localDiag = makeObject<DiagnosticsManager>();

    auto vsData = FileSystem::readAllBytes(vertexShaderFilePath);
    auto psData = FileSystem::readAllBytes(pixelShaderFilePath);

	{
#ifdef LN_BUILD_EMBEDDED_SHADER_TRANSCOMPILER
		List<Path> includeDirs;
		if (properties) {
			for (auto& path : properties->m_includeDirectories)
				includeDirs.add(path);
		}

		detail::UnifiedShaderCompiler compiler(m_manager, localDiag);
		if (!compiler.compileSingleCodes(
			reinterpret_cast<const char*>(vsData.data()), vsData.size(), "main",
			reinterpret_cast<const char*>(psData.data()), psData.size(), "main",
			includeDirs, {})) {
			LN_ERROR();
			return;
		}
		if (!compiler.link()) {
			LN_ERROR();
			return;
		}

		createFromUnifiedShader(compiler.unifiedShader(), localDiag);
#else
        LN_NOTIMPLEMENTED();
        return;
#endif
	}
    //createSinglePassShader(
    //    reinterpret_cast<const char*>(vsData.data()), vsData.size(), reinterpret_cast<const char*>(psData.data()), psData.size(), localDiag, properties);

    postInitialize();
    m_name = Path(vertexShaderFilePath).fileNameWithoutExtension();
    m_name += u",";
    m_name += Path(pixelShaderFilePath).fileNameWithoutExtension();

    if (!properties || !properties->m_diag) {
        if (localDiag->hasError()) {
            LN_ERROR(localDiag->toString());
            return;
        } else if (localDiag->hasWarning()) {
            LN_LOG_WARNING << localDiag->toString();
        }
    }
}

void Shader::init(const String& name, Stream* stream)
{
    Shader::init();
    Ref<DiagnosticsManager> localDiag = makeObject<DiagnosticsManager>();

    createFromStream(stream, localDiag);

    postInitialize();
    m_name = name;

    if (localDiag->hasError()) {
        LN_LOG_ERROR << localDiag->toString();
        LN_ERROR(name);
        return;
    } else if (localDiag->hasWarning()) {
        LN_LOG_WARNING << localDiag->toString();
    }
}

void Shader::createFromStream(Stream* stream, DiagnosticsManager* diag)
{
    detail::UnifiedShader unifiedShader(diag);
    if (unifiedShader.load(stream)) {
		createFromUnifiedShader(&unifiedShader, diag);
    }
}

void Shader::createFromUnifiedShader(detail::UnifiedShader* unifiedShader, DiagnosticsManager* diag)
{
    m_descriptorLayout = makeObject<ShaderDescriptorLayout>(unifiedShader->globalDescriptorLayout());
    m_descriptor = makeObject<ShaderDescriptor>(this);

	for (int iTech = 0; iTech < unifiedShader->techniqueCount(); iTech++) {
		detail::UnifiedShader::TechniqueId techId = unifiedShader->techniqueId(iTech);
		auto tech = makeObject<ShaderTechnique>(String::fromStdString(unifiedShader->techniqueName(techId)));
		tech->setOwner(this);
		m_techniques->add(tech);

		int passCount = unifiedShader->getPassCountInTechnique(techId);
		for (int iPass = 0; iPass < passCount; iPass++) {
			detail::UnifiedShader::PassId passId = unifiedShader->getPassIdInTechnique(techId, iPass);

			auto rhiPass = detail::GraphicsResourceInternal::manager(this)->deviceContext()->createShaderPassFromUnifiedShaderPass(unifiedShader, passId, diag);
			if (rhiPass) {
				auto pass = makeObject<ShaderPass>(
                    String::fromStdString(unifiedShader->passName(passId)),
                    rhiPass,
                    unifiedShader->renderState(passId),
                    unifiedShader->descriptorLayout(passId),
                    m_descriptorLayout);
				tech->addShaderPass(pass);
				pass->setupParameters();
			}
		}

        tech->setupSemanticsManager();
	}
}

void Shader::onDispose(bool explicitDisposing)
{
    for (auto& tech : m_techniques) {
        for (auto& pass : tech->passes()) {
            pass->dispose();
        }
    }
    m_techniques->clear();

    GraphicsResource::onDispose(explicitDisposing);
}

void Shader::onChangeDevice(detail::IGraphicsDevice* device)
{
    LN_NOTIMPLEMENTED();
}

void Shader::postInitialize()
{
    // find global constant buffer.
    {
        m_globalConstantBuffer = findConstantBuffer(u"_Global");
        //if (!m_globalConstantBuffer && !m_buffers.isEmpty()) {
        //    m_globalConstantBuffer = m_buffers.front();
        //}

        for (auto& cbuffer : m_buffers) {
            m_semanticsManager.prepareConstantBuffer(cbuffer);
        }

    }

    // analyze semantices.
    {
        if (m_globalConstantBuffer) {
            for (auto& param : m_globalConstantBuffer->m_parameters) {
                m_semanticsManager.prepareParameter(param);
            }
        }

        for (auto& param : m_textureParameters) {
            m_semanticsManager.prepareParameter(param);
        }
    }
}

#if 0
void Shader::setBool(const StringRef& name, bool value)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setBool(value);
}

void Shader::setInt(const StringRef& name, int value)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setInt(value);
}

void Shader::setBoolArray(const StringRef& name, const bool* value, int count)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setBoolArray(value, count);
}

void Shader::setFloat(const StringRef& name, float value)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setFloat(value);
}

void Shader::setFloatArray(const StringRef& name, const float* value, int count)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setFloatArray(value, count);
}

void Shader::setVector(const StringRef& name, const Vector4& value)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setVector(value);
}

void Shader::setVectorArray(const StringRef& name, const Vector4* value, int count)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setVectorArray(value, count);
}

void Shader::setMatrix(const StringRef& name, const Matrix& value)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setMatrix(value);
}

void Shader::setMatrixArray(const StringRef& name, const Matrix* value, int count)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setMatrixArray(value, count);
}

void Shader::setTexture(const StringRef& name, Texture* value)
{
	ShaderParameter* param = findParameter(name);
	if (param) param->setTexture(value);
}
#endif

//ShaderParameter* Shader::findParameter(const StringRef& name)
//{
//	for (auto& param : m_parameters)
//	{
//		if (param->name() == name)
//		{
//			return param;
//		}
//	}
//	return nullptr;
//}

ShaderParameter* Shader::findParameter(const StringRef& name) const
{
    // find global constant buffer.
    if (m_globalConstantBuffer) {
        ShaderParameter* result = m_globalConstantBuffer->findParameter(name);
        if (result) {
            return result;
        }
    }

    // find texture params.
    {
        auto result = m_textureParameters.findIf([&](const Ref<ShaderParameter>& param) { return param->name() == name; });
        return (result) ? *result : nullptr;
    }
}

ShaderConstantBuffer* Shader::findConstantBuffer(const StringRef& name) const
{
    auto result = m_buffers.findIf([name](const ShaderConstantBuffer* buf) { return buf->name() == name; });
    return (result) ? *result : nullptr;
}

ShaderTechnique* Shader::findTechnique(const StringRef& name) const
{
    for (auto& var : m_techniques) {
        if (String::compare(StringRef(var->name()), 0, StringRef(name), 0, -1, CaseSensitivity::CaseSensitive) == 0) {
            return var;
        }
    }
    return nullptr;
}

Ref<ReadOnlyList<Ref<ShaderTechnique>>> Shader::techniques() const
{
    return m_techniques;
}

//Ref<ShaderDescriptor> Shader::createDescriptor()
//{
//    return makeObject<ShaderDescriptor>(this);
//}

// TODO: 名前の指定方法をもう少しいい感じにしたい。ImageEffect を Forward_Geometry_UnLighting と書かなければならないなど、煩雑。
ShaderTechnique* Shader::findTechniqueByClass(const detail::ShaderTechniqueClass& techniqueClass) const
{
    ShaderTechnique* defaultTech = nullptr;
    for (auto& tech : m_techniques) {
        if (detail::ShaderTechniqueClass::equals(detail::ShaderHelper::techniqueClass(tech), techniqueClass)) {
            return tech;
        }
        if (detail::ShaderHelper::techniqueClass(tech).defaultTechnique) {
            defaultTech = tech;
        }
    }

    if (defaultTech) {
        return defaultTech;
    }

    return nullptr;
}

ShaderConstantBuffer* Shader::getOrCreateConstantBuffer(detail::IShaderUniformBuffer* rhiBuffer)
{
    for (auto& buffer : m_buffers) {
        if (buffer->asciiName() == rhiBuffer->name()) {
            LN_CHECK(rhiBuffer->bufferSize() == buffer->size());
            return buffer;
        }
    }

    auto buffer = makeObject<ShaderConstantBuffer>(this, rhiBuffer);
    m_buffers.add(buffer);
    return buffer;
}

ShaderParameter* Shader::getOrCreateTextureParameter(const String& name)
{
    auto result = m_textureParameters.findIf([&](const Ref<ShaderParameter>& param) { return param->name() == name; });
    if (result) {
        return *result;
    } else {
        auto param = makeObject<ShaderParameter>(ShaderParameterClass::Texture, name);
        m_textureParameters.add(param);
        return param;
    }
}

//=============================================================================
// ShaderParameter

ShaderParameter::ShaderParameter()
    : m_owner(nullptr)
    , m_class(ShaderParameterClass::Constant)
    , m_desc()
    , m_name()
    , m_textureValue(nullptr)
{
}

void ShaderParameter::init(ShaderConstantBuffer* owner, const detail::ShaderUniformTypeDesc& desc, const String& name)
{
    Object::init();
    m_owner = owner;
    m_desc = desc;
    m_name = name;
}

void ShaderParameter::init(ShaderParameterClass parameterClass, const String& name)
{
    Object::init();
    m_class = parameterClass;
    m_name = name;
}

void ShaderParameter::onDispose(bool explicitDisposing)
{
    Object::onDispose(explicitDisposing);
}

void ShaderParameter::setInt(int value)
{
    alignScalarsToBuffer((const byte_t*)&value, sizeof(int), 1, m_owner->buffer().data(), m_desc.offset, 1, 0);
}

void ShaderParameter::setIntArray(const int* value, int count)
{
    alignScalarsToBuffer((const byte_t*)value, sizeof(int), count, m_owner->buffer().data(), m_desc.offset, m_desc.elements, m_desc.arrayStride);
}

void ShaderParameter::setFloat(float value)
{
    alignScalarsToBuffer((const byte_t*)&value, sizeof(float), 1, m_owner->buffer().data(), m_desc.offset, 1, 0);
}

void ShaderParameter::setFloatArray(const float* value, int count)
{
    alignScalarsToBuffer((const byte_t*)value, sizeof(float), count, m_owner->buffer().data(), m_desc.offset, m_desc.elements, m_desc.arrayStride);
}

void ShaderParameter::setVector(const Vector4& value)
{
    alignVectorsToBuffer((const byte_t*)&value, 4, 1, m_owner->buffer().data(), m_desc.offset, 1, 0, m_desc.columns);
}

void ShaderParameter::setVectorArray(const Vector4* value, int count)
{
    alignVectorsToBuffer((const byte_t*)value, 4, count, m_owner->buffer().data(), m_desc.offset, m_desc.elements, m_desc.arrayStride, m_desc.columns);
}

void ShaderParameter::setMatrix(const Matrix& value)
{
    alignMatricesToBuffer((const byte_t*)&value, 4, 4, 1, m_owner->buffer().data(), m_desc.offset, 1, m_desc.matrixStride, 0, m_desc.rows, m_desc.columns, true);
}

void ShaderParameter::setMatrixArray(const Matrix* value, int count)
{
    alignMatricesToBuffer((const byte_t*)value, 4, 4, count, m_owner->buffer().data(), m_desc.offset, m_desc.elements, m_desc.matrixStride, m_desc.arrayStride, m_desc.rows, m_desc.columns, true);
}

void ShaderParameter::setTexture(Texture* value)
{
    m_textureValue = value;
}

#if 0
void ShaderParameter::setBool(bool value)
{
	m_value.setBool(value);
}


void ShaderParameter::setBoolArray(const bool* value, int count)
{
	m_value.setBoolArray(value, count);
}
void ShaderParameter::setTexture(Texture* value)
{
	m_value.setTexture(value);
}

void ShaderParameter::setPointer(void* value)
{
	m_value.setPointer(value);
}
#endif

//=============================================================================
// ShaderConstantBuffer

ShaderConstantBuffer::ShaderConstantBuffer()
    : m_owner(nullptr)
    , m_name()
    , m_asciiName()
    , m_buffer()
    , m_parameters()
{
}

void ShaderConstantBuffer::init(Shader* owner, detail::IShaderUniformBuffer* rhiObject)
{
    Object::init();
    m_owner = owner;
    m_name = String::fromStdString(rhiObject->name());
    m_asciiName = rhiObject->name();
    m_buffer.resize(rhiObject->bufferSize());

    for (int i = 0; i < rhiObject->getUniformCount(); i++) {
        detail::IShaderUniform* field = rhiObject->getUniform(i);
        m_parameters.add(makeObject<ShaderParameter>(this, field->desc(), String::fromStdString(field->name())));
    }
}

void ShaderConstantBuffer::setData(const void* data, int size)
{
    m_buffer.assign(data, size);

    for (const auto& param : m_parameters) {
        const auto& desc = param->desc();
        if (desc.type2 == detail::ShaderUniformType_Matrix &&
            desc.columns == 4 && desc.rows == 4) {
            Matrix* m = reinterpret_cast<Matrix*>(m_buffer.data() + desc.offset);
            m->transpose();
        }
    }
}

ShaderParameter* ShaderConstantBuffer::findParameter(const StringRef& name) const
{
    auto result = m_parameters.findIf([name](const ShaderParameter* param) { return param->name() == name; });
    return (result) ? *result : nullptr;
}

void ShaderConstantBuffer::commit(GraphicsContext* graphicsContext, detail::IShaderUniformBuffer* rhiObject)
{
    detail::RenderBulkData data = detail::GraphicsContextInternal::getRenderingCommandList(graphicsContext)->allocateBulkData(m_buffer.size());
    memcpy(data.writableData(), m_buffer.data(), data.size());

    LN_ENQUEUE_RENDER_COMMAND_2(
        ShaderConstantBuffer_commit, graphicsContext, detail::RenderBulkData, data, Ref<detail::IShaderUniformBuffer>, rhiObject, {
            rhiObject->setData(data.data(), data.size());
        });
}

//=============================================================================
// ShaderTechnique

ShaderTechnique::ShaderTechnique()
    : m_passes(makeList<Ref<ShaderPass>>())
{
}

ShaderTechnique::~ShaderTechnique()
{
}

void ShaderTechnique::init(const String& name)
{
    Object::init();
    m_name = name;
    detail::ShaderTechniqueClass::parseTechniqueClassString(m_name, &m_techniqueClass);
}

void ShaderTechnique::setupSemanticsManager()
{
    m_semanticsManager = std::make_unique<detail::ShaderTechniqueSemanticsManager>();
    m_semanticsManager->init(this);
}

Ref<ReadOnlyList<Ref<ShaderPass>>> ShaderTechnique::passes() const
{
    return m_passes;
}

void ShaderTechnique::addShaderPass(ShaderPass* pass)
{
    m_passes->add(pass);
    pass->setOwner(this);
}

//=============================================================================
// ShaderPass

ShaderPass::ShaderPass()
    : m_owner(nullptr)
    , m_name()
    , m_rhiPass(nullptr)
    , m_bufferEntries()
    , m_textureParameters()
    , m_renderState(nullptr)
{
}

ShaderPass::~ShaderPass()
{
}

void ShaderPass::init(const String& name, detail::IShaderPass* rhiPass, detail::ShaderRenderState* renderState, const detail::DescriptorLayout& layout, const ShaderDescriptorLayout* globalLayout)
{
    if (LN_REQUIRE(rhiPass)) return;
    Object::init();

    m_name = name;
    m_rhiPass = rhiPass;
    m_renderState = renderState;

    m_descriptorLayout.init(layout, globalLayout);

}

void ShaderPass::onDispose(bool explicitDisposing)
{
    m_rhiPass = nullptr;

    Object::onDispose(explicitDisposing);
}

Shader* ShaderPass::shader() const
{
    return m_owner->shader();
}

void ShaderPass::setupParameters()
{
    m_bufferEntries.clear();

    for (int i = 0; i < m_rhiPass->getUniformBufferCount(); i++) {
        detail::IShaderUniformBuffer* rhi = m_rhiPass->getUniformBuffer(i);
        ShaderConstantBuffer* buf = m_owner->shader()->getOrCreateConstantBuffer(rhi);
        m_bufferEntries.add({buf, Ref<detail::IShaderUniformBuffer>(rhi)});
    }

    m_textureParameters.clear();
    detail::IShaderSamplerBuffer* samplerBuffer = m_rhiPass->samplerBuffer();
    if (samplerBuffer) {
        for (int i = 0; i < samplerBuffer->registerCount(); i++) {
            ShaderParameter* param = m_owner->shader()->getOrCreateTextureParameter(String::fromStdString(samplerBuffer->getTextureRegisterName(i)));
            m_textureParameters.add(param);
        }
    }
}

void ShaderPass::commitContantBuffers(GraphicsContext* graphicsContext, bool* outModified)
{
    for (auto& e : m_bufferEntries) {
        e.buffer->commit(graphicsContext, e.rhiObject);
    }

    // TODO: 1つのバッファにまとめるとか、一括で送りたい。
    detail::IShaderSamplerBuffer* samplerBuffer = m_rhiPass->samplerBuffer();
    if (samplerBuffer) {
		for (int i = 0; i < samplerBuffer->registerCount(); i++) {
			auto* manager = detail::GraphicsResourceInternal::manager(m_owner->shader());;
			Texture* texture = m_textureParameters[i]->texture();

			if (texture) {
#ifdef LN_DEBUG	// 検証。描画先とサンプラに同時に同じテクスチャは使えない
				{
					const auto& renderPass = detail::GraphicsContextInternal::currentRenderPass(graphicsContext);
					for (int i = 0; i < detail::MaxMultiRenderTargets; i++) {
						if (renderPass->renderTarget(i) == texture) {
							LN_ERROR();
							return;
						}
					}
				}
#endif

#if 1
				SamplerState* sampler = nullptr;
				if (texture && texture->samplerState()) {
					sampler = texture->samplerState();
				}
				else {
					sampler = manager->defaultSamplerState();
				}

				bool modified = false;
				detail::ITexture* rhiTexture = detail::GraphicsResourceInternal::resolveRHIObject<detail::ITexture>(graphicsContext, texture, &modified);
                *outModified |= modified;
				detail::ISamplerState* rhiSampler = detail::GraphicsResourceInternal::resolveRHIObject<detail::ISamplerState>(graphicsContext, sampler, &modified);
                *outModified |= modified;
				LN_ENQUEUE_RENDER_COMMAND_4(
					ShaderConstantBuffer_commit_setTexture, graphicsContext, detail::IShaderSamplerBuffer*, samplerBuffer, int, i, Ref<detail::ITexture>, rhiTexture, Ref<detail::ISamplerState>, rhiSampler, {
						samplerBuffer->setTexture(i, rhiTexture);
						samplerBuffer->setSamplerState(i, rhiSampler);
				});
#else
				if (texture && texture->samplerState()) {
					SamplerState* sampler = texture->samplerState();
					detail::ITexture* rhiTexture = (texture) ? texture->resolveRHIObject() : nullptr;
					detail::ISamplerState* rhiSampler = (sampler) ? sampler->resolveRHIObject() : nullptr;
					LN_ENQUEUE_RENDER_COMMAND_4(
						ShaderConstantBuffer_commit_setTexture, manager, detail::IShaderSamplerBuffer*, samplerBuffer, int, i, Ref<detail::ITexture>, rhiTexture, Ref<detail::ISamplerState>, rhiSampler, {
							samplerBuffer->setTexture(i, rhiTexture);
							samplerBuffer->setSamplerState(i, rhiSampler);
						});
				}
				else {
					detail::ITexture* rhiTexture = (texture) ? texture->resolveRHIObject() : nullptr;
					LN_ENQUEUE_RENDER_COMMAND_3(
						ShaderConstantBuffer_commit_setTexture, manager, detail::IShaderSamplerBuffer*, samplerBuffer, int, i, Ref<detail::ITexture>, rhiTexture, {
							samplerBuffer->setTexture(i, rhiTexture);
						});
				}
#endif
			}
			else {
				// TODO: SamplerState のみの設定
			}
        }
    }
}

detail::IShaderPass* ShaderPass::resolveRHIObject(GraphicsContext* graphicsContext, bool* outModified)
{
    commitContantBuffers(graphicsContext, outModified);
    return m_rhiPass;
}

void ShaderPass::submitShaderDescriptor(GraphicsContext* graphicsContext, detail::ICommandList* commandList, const ShaderDescriptor* descripter, bool* outModified)
{
    if (descripter) {
        if (descripter != m_lastShaderDescriptor || m_lastShaderDescriptorRevision != descripter->m_revision) {

            {
                auto* manager = detail::GraphicsResourceInternal::manager(m_owner->shader());
                const ShaderDescriptorLayout* globalLayout = m_owner->m_owner->descriptorLayout();
                detail::ShaderDescriptorTableUpdateInfo updateInfo;

                // まず UniformBuffer に必要なサイズを測る
                size_t totalSize = 0;
                for (int i = 0; i < m_descriptorLayout.m_buffers.size(); i++) {
                    if (i >= detail::ShaderDescriptorTableUpdateInfo::MaxElements) {
                        LN_NOTIMPLEMENTED();
                        break;
                    }
                    updateInfo.uniforms[i].size = globalLayout->m_buffers[m_descriptorLayout.m_buffers[i].dataIndex].size;
                    totalSize += updateInfo.uniforms[i].size;
                }

                // UniformBuffer に必要な領域をまとめて確保してデータコピー
                detail::RenderBulkData uniformBufferData = detail::GraphicsContextInternal::getRenderingCommandList(graphicsContext)->allocateBulkData(totalSize);
                size_t offset = 0;
                for (int i = 0; i < m_descriptorLayout.m_buffers.size(); i++) {
                    auto& view = updateInfo.uniforms[i];
                    void* d = static_cast<byte_t*>(uniformBufferData.writableData()) + offset;
                    memcpy(d, descripter->m_buffers[m_descriptorLayout.m_buffers[i].dataIndex].data(), view.size);
                    view.data = d;
                    offset += view.size;
                }

                // Textures
                for (int i = 0; i < m_descriptorLayout.m_textures.size(); i++) {
                    if (i >= detail::ShaderDescriptorTableUpdateInfo::MaxElements) {
                        LN_NOTIMPLEMENTED();
                        break;
                    }
                    const auto& info = m_descriptorLayout.m_textures[i];
                    Texture* texture = descripter->m_textures[info.dataIndex];
                    if (!texture) {
                        texture = manager->whiteTexture();
                    }

                    SamplerState* sampler = nullptr;
                    if (texture->samplerState())
                        sampler = texture->samplerState();
                    else
                        sampler = manager->defaultSamplerState();

                    bool modified = false;
                    auto& view = updateInfo.textures[i];
                    view.texture = detail::GraphicsResourceInternal::resolveRHIObject<detail::ITexture>(graphicsContext, texture, &modified);
                    view.stamplerState = detail::GraphicsResourceInternal::resolveRHIObject<detail::ISamplerState>(graphicsContext, sampler, &modified);
                    (*outModified) |= modified;
                }

                // Samplers
                for (int i = 0; i < m_descriptorLayout.m_samplers.size(); i++) {
                    if (i >= detail::ShaderDescriptorTableUpdateInfo::MaxElements) {
                        LN_NOTIMPLEMENTED();
                        break;
                    }
                    const auto& info = m_descriptorLayout.m_samplers[i];
                    SamplerState* sampler = descripter->m_samplers[info.dataIndex];
                    if (!sampler)
                        sampler = manager->defaultSamplerState();

                    bool modified = false;
                    auto& view = updateInfo.samplers[i];
                    view.texture = nullptr;
                    view.stamplerState = detail::GraphicsResourceInternal::resolveRHIObject<detail::ISamplerState>(graphicsContext, sampler, &modified);
                    (*outModified) |= modified;
                }

                detail::IShaderDescriptorTable* rhiDescriptorTable = m_rhiPass->descriptorTable();

                LN_ENQUEUE_RENDER_COMMAND_3(
                    ShaderConstantBuffer_submitShaderDescriptor, graphicsContext,
                    detail::ICommandList*, commandList,
                    detail::IShaderDescriptorTable*, rhiDescriptorTable,
                    detail::ShaderDescriptorTableUpdateInfo, updateInfo,
                    {
                        commandList->setDescriptorTableData(rhiDescriptorTable, &updateInfo);
                    });
            }


            m_lastShaderDescriptor = m_lastShaderDescriptor;
            m_lastShaderDescriptorRevision = descripter->m_revision;
        }
    }
    else {
        m_lastShaderDescriptor = nullptr;
        m_lastShaderDescriptorRevision = 0;
    }

}

//=============================================================================
// ShaderDescriptor

ShaderDescriptor::ShaderDescriptor()
{
}

bool ShaderDescriptor::init(Shader* ownerShader)
{
    if (LN_REQUIRE(ownerShader)) return false;
    if (!Object::init()) return false;

    m_ownerShader = ownerShader;

    const auto& layout = m_ownerShader->descriptorLayout();

    m_buffers.resize(layout->m_buffers.size());
    for (int i = 0; i < layout->m_buffers.size(); i++) {
        m_buffers[i].resize(layout->m_buffers[i].size);
    }

    m_textures.resize(layout->textureRegisterCount());
    m_samplers.resize(layout->samplerRegisterCount());

    return true;
}

ShaderDescriptorLayout* ShaderDescriptor::descriptorLayout() const
{
    return m_ownerShader->descriptorLayout();
}

//int ShaderDescriptor::findUniformBufferIndex(const ln::StringRef& name) const
//{
//    return m_ownerShader->descriptorLayout()->findUniformBufferRegisterIndex(name);
//}
//
//int ShaderDescriptor::findTextureIndex(const ln::StringRef& name) const
//{
//    return m_ownerShader->descriptorLayout()->findTextureRegisterIndex(name);
//}
//
//int ShaderDescriptor::findSamplerIndex(const ln::StringRef& name) const
//{
//    return m_ownerShader->descriptorLayout()->findSamplerRegisterIndex(name);
//}


void ShaderDescriptor::setData(int uniformBufferIndex, const void* data, size_t size)
{
    auto& buffer = m_buffers[uniformBufferIndex];

    buffer.assign(data, size);

    // TODO: Shader 側で行優先にするべきかも…
    for (const auto& member : descriptorLayout()->m_buffers[uniformBufferIndex].members) {
        const auto& desc = descriptorLayout()->m_members[member].desc;//param->desc();
        if (desc.type2 == detail::ShaderUniformType_Matrix &&
            desc.columns == 4 && desc.rows == 4) {
            Matrix* m = reinterpret_cast<Matrix*>(buffer.data() + desc.offset);
            m->transpose();
        }
    }
}

void ShaderDescriptor::setInt(int memberIndex, int value)
{
    const auto& member = descriptorLayout()->m_members[memberIndex];
    auto& buffer = m_buffers[member.uniformBufferRegisterIndex];
    alignScalarsToBuffer((const byte_t*)&value, sizeof(int), 1, buffer.data(), member.desc.offset, 1, 0);
}

void ShaderDescriptor::setIntArray(int memberIndex, const int* value, int count)
{
    const auto& member = descriptorLayout()->m_members[memberIndex];
    auto& buffer = m_buffers[member.uniformBufferRegisterIndex];
    alignScalarsToBuffer((const byte_t*)value, sizeof(int), count, buffer.data(), member.desc.offset, member.desc.elements, member.desc.arrayStride);
}

void ShaderDescriptor::setFloat(int memberIndex, float value)
{
    const auto& member = descriptorLayout()->m_members[memberIndex];
    auto& buffer = m_buffers[member.uniformBufferRegisterIndex];
    alignScalarsToBuffer((const byte_t*)&value, sizeof(float), 1, buffer.data(), member.desc.offset, 1, 0);
}

void ShaderDescriptor::setFloatArray(int memberIndex, const float* value, int count)
{
    const auto& member = descriptorLayout()->m_members[memberIndex];
    auto& buffer = m_buffers[member.uniformBufferRegisterIndex];
    alignScalarsToBuffer((const byte_t*)value, sizeof(float), count, buffer.data(), member.desc.offset, member.desc.elements, member.desc.arrayStride);
}

void ShaderDescriptor::setVector(int memberIndex, const Vector4& value)
{
    const auto& member = descriptorLayout()->m_members[memberIndex];
    auto& buffer = m_buffers[member.uniformBufferRegisterIndex];
    alignVectorsToBuffer((const byte_t*)&value, 4, 1, buffer.data(), member.desc.offset, 1, 0, member.desc.columns);
}

void ShaderDescriptor::setVectorArray(int memberIndex, const Vector4* value, int count)
{
    const auto& member = descriptorLayout()->m_members[memberIndex];
    auto& buffer = m_buffers[member.uniformBufferRegisterIndex];
    alignVectorsToBuffer((const byte_t*)value, 4, count, buffer.data(), member.desc.offset, member.desc.elements, member.desc.arrayStride, member.desc.columns);
}

void ShaderDescriptor::setMatrix(int memberIndex, const Matrix& value)
{
    const auto& member = descriptorLayout()->m_members[memberIndex];
    auto& buffer = m_buffers[member.uniformBufferRegisterIndex];
    alignMatricesToBuffer((const byte_t*)&value, 4, 4, 1, buffer.data(), member.desc.offset, 1, member.desc.matrixStride, 0, member.desc.rows, member.desc.columns, true);
}

void ShaderDescriptor::setMatrixArray(int memberIndex, const Matrix* value, int count)
{
    const auto& member = descriptorLayout()->m_members[memberIndex];
    auto& buffer = m_buffers[member.uniformBufferRegisterIndex];
    alignMatricesToBuffer((const byte_t*)value, 4, 4, count, buffer.data(), member.desc.offset, member.desc.elements, member.desc.matrixStride, member.desc.arrayStride, member.desc.rows, member.desc.columns, true);
}

void ShaderDescriptor::setTexture(int textureIndex, Texture* value)
{
    m_textures[textureIndex] = value;
}

void ShaderDescriptor::setSampler(int textureIndex, Texture* value)
{
    m_textures[textureIndex] = value;
}

void ShaderDescriptor::setSamplerState(int samplerIndex, SamplerState* value)
{
    m_samplers[samplerIndex] = value;
}

//=============================================================================
// ShaderDescriptorLayout

ShaderDescriptorLayout::ShaderDescriptorLayout()
{
}

bool ShaderDescriptorLayout::init(const detail::DescriptorLayout& layout)
{
    if (!Object::init()) return false;

    // 'b'
    m_buffers.resize(layout.uniformBufferRegister.size());
    for (int i = 0; i < layout.uniformBufferRegister.size(); i++) {
        const auto& item = layout.uniformBufferRegister[i];
        m_buffers[i].name = String::fromStdString(item.name);
        m_buffers[i].size = item.size;

        for (const auto& member : item.members) {
            detail::ShaderUniformTypeDesc desc;
            desc.type2 = static_cast<detail::ShaderUniformType>(member.type);

            desc.rows = member.matrixRows;
            desc.columns = member.matrixColumns;
            desc.elements = member.arrayElements;
            if (desc.columns == 0) { // OpenGL Dirver の動作に合わせる
                desc.columns = member.vectorElements;// *sizeof(float);
            }

            desc.offset = member.offset;
            if (member.arrayElements > 0) {
                detail::ShaderHelper::resolveStd140Layout(member, &desc.arrayStride);
            }
            desc.matrixStride = member.matrixColumns * sizeof(float);


            UniformMemberInfo memberInfo;
            memberInfo.uniformBufferRegisterIndex = i;
            memberInfo.name = String::fromStdString(member.name);
            memberInfo.desc = desc;
            m_members.add(memberInfo);

            m_buffers[i].members.push_back(m_members.size() - 1);
        }
    }

    // 't'
    m_textures.resize(layout.textureRegister.size());
    for (int i = 0; i < layout.textureRegister.size(); i++) {
        const auto& item = layout.textureRegister[i];
        m_textures[i].name = String::fromStdString(item.name);
    }

    // 's'
    m_samplers.resize(layout.samplerRegister.size());
    for (int i = 0; i < layout.samplerRegister.size(); i++) {
        const auto& item = layout.samplerRegister[i];
        m_samplers[i].name = String::fromStdString(item.name);
    }

    return true;
}

int ShaderDescriptorLayout::findUniformBufferRegisterIndex(const ln::StringRef& name) const
{
    return m_buffers.indexOfIf([&](const auto& x) { return x.name == name; });
}

int ShaderDescriptorLayout::findUniformMemberIndex(const ln::StringRef& name) const
{
    return m_members.indexOfIf([&](const auto& x) { return x.name == name; });
}

int ShaderDescriptorLayout::findTextureRegisterIndex(const ln::StringRef& name) const
{
    return m_textures.indexOfIf([&](const auto& x) { return x.name == name; });
}

int ShaderDescriptorLayout::findSamplerRegisterIndex(const ln::StringRef& name) const
{
    return m_samplers.indexOfIf([&](const auto& x) { return x.name == name; });
}

//=============================================================================
// ShaderPassDescriptorLayout

void ShaderPassDescriptorLayout::init(const detail::DescriptorLayout& layout, const ShaderDescriptorLayout* globalLayout)
{
    m_buffers.resize(layout.uniformBufferRegister.size());
    for (int i = 0; i < layout.uniformBufferRegister.size(); i++) {
        const auto& item = layout.uniformBufferRegister[i];
        m_buffers[i].dataIndex = globalLayout->findUniformBufferRegisterIndex(String::fromStdString(item.name));
        m_buffers[i].bindingIndex = item.binding;
        m_buffers[i].stageFlags = item.stageFlags;

        if (LN_ENSURE(m_buffers[i].dataIndex >= 0)) return;
        if (LN_ENSURE(m_buffers[i].bindingIndex >= 0)) return;
        if (LN_ENSURE(m_buffers[i].stageFlags != 0)) return;
    }

    m_textures.resize(layout.textureRegister.size());
    for (int i = 0; i < layout.textureRegister.size(); i++) {
        const auto& item = layout.textureRegister[i];
        m_textures[i].dataIndex = globalLayout->findTextureRegisterIndex(String::fromStdString(item.name));
        m_textures[i].bindingIndex = item.binding;
        m_textures[i].stageFlags = item.stageFlags;

        if (LN_ENSURE(m_textures[i].dataIndex >= 0)) return;
        if (LN_ENSURE(m_textures[i].bindingIndex >= 0)) return;
        if (LN_ENSURE(m_textures[i].stageFlags != 0)) return;
    }

    m_samplers.resize(layout.samplerRegister.size());
    for (int i = 0; i < layout.samplerRegister.size(); i++) {
        const auto& item = layout.samplerRegister[i];
        m_samplers[i].dataIndex = globalLayout->findSamplerRegisterIndex(String::fromStdString(item.name));
        m_samplers[i].bindingIndex = item.binding;
        m_samplers[i].stageFlags = item.stageFlags;

        if (LN_ENSURE(m_samplers[i].dataIndex >= 0)) return;
        if (LN_ENSURE(m_samplers[i].bindingIndex >= 0)) return;
        if (LN_ENSURE(m_samplers[i].stageFlags != 0)) return;
    }
}

} // namespace ln
