﻿
#include "Internal.hpp"
#include "AssetManager.hpp"
#include <LuminoEngine/Asset/AssetObject.hpp>
#include <LuminoEngine/Asset/Assets.hpp>

// TODO: for importer
#include <LuminoEngine/Graphics/Texture.hpp>

namespace ln {

//=============================================================================
// Asset

bool Assets::existsFile(const StringRef& filePath)
{
    return detail::EngineDomain::assetManager()->existsFile(filePath);
}

//Ref<Texture2D> Assets::loadTexture(const StringRef& filePath)
//{
//    return detail::EngineDomain::assetManager()->loadTexture(filePath);
//}

Ref<Shader> Assets::loadShader(const StringRef& filePath)
{
    return detail::EngineDomain::assetManager()->loadShader(filePath);
}

Ref<ByteBuffer> Assets::readAllBytes(const StringRef& filePath)
{
	return detail::EngineDomain::assetManager()->readAllBytes(filePath);
}

Ref<Object> Assets::loadAsset(const StringRef& filePath)
{
    return detail::EngineDomain::assetManager()->loadAsset(filePath);
}

Ref<Stream> Assets::openFileStream(const StringRef& filePath)
{
    return detail::EngineDomain::assetManager()->openFileStream(filePath);
}

void Assets::serializeAsAssetPathInternal(Archive& ar, const StringRef& name, Ref<Object>& value)
{
    String localPath;
    if (ar.isSaving()) {
        localPath = detail::AssetManager::makeRelativeAssetPath(ar.basePath(), value->assetPath());
    }

    ar & makeNVP(name, localPath);

    if (ar.isLoading()) {
        auto assetPath = detail::AssetManager::combineAssetPath(ar.basePath(), localPath);
        //value = dynamic_pointer_cast<Tileset>(Assets::loadAsset(assetPath));
        auto assetModel = detail::EngineDomain::assetManager()->loadAssetModelFromAssetPath(assetPath);
        value = assetModel->target();
        if (value) {
            value->setAssetPath(assetPath);
        }
    }
}

//=============================================================================
// AssetImporter

AssetImporter::AssetImporter()
{
}

AssetImporter::~AssetImporter()
{
}

bool AssetImporter::testSupportedExtensions(const ln::Path& sourceFilePath)
{
    List<String> exts;
    onGetSupportedExtensions(&exts);
    return exts.containsIf([&](auto& x) { return sourceFilePath.hasExtension(x); });
}

Ref<AssetModel> AssetImporter::import(const ln::Path& sourceFilePath)
{
    return onImport(sourceFilePath);
}

//bool AssetImporter::onImport(const ln::Path& sourceFilePath)
//{
//
//}

////=============================================================================
//// TextureImporter
//
//TextureImporter::TextureImporter()
//{
//}
//
//void TextureImporter::onGetSupportedExtensions(List<String>* outExtensions)
//{
//    *outExtensions = { u"png" };
//}
//
//Ref<AssetModel> TextureImporter::onImport(const ln::Path& sourceFilePath)
//{
//    auto texture = makeObject<Texture2D>(sourceFilePath);
//    auto asset = makeObject<AssetModel>(texture);
//    asset->saveInternal(sourceFilePath.str() + AssetModel::AssetFileExtension);
//    return asset;
//}

} // namespace ln

