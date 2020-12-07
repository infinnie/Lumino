﻿
#include "Internal.hpp"
#include <LuminoEngine/Mesh/MeshModel.hpp>
#include "MeshManager.hpp"
#include "MeshImporter.hpp"

namespace ln {
namespace detail {

MeshImporter::MeshImporter()
	: m_applyBoneTransformationsEnabled(true)
	//, m_flipZCoordinate(true)
	, m_settings(nullptr)
{
}

void MeshImporter::prepare(MeshManager* meshManager, DiagnosticsManager* diag, const MeshImportSettings* settings)
{
	m_meshManager = meshManager;
	m_assetManager = m_meshManager->assetManager();
	m_diag = diag;
	m_settings = settings;

	m_applyBoneTransformationsEnabled = settings->applyBoneTransformationsEnabled().valueOr(m_applyBoneTransformationsEnabled);

	if (m_settings->isCharacterModelFormat()) {
		m_isCharacterModelFormat = m_settings->isCharacterModelFormat().value();
	}
}

} // namespace detail
} // namespace ln
