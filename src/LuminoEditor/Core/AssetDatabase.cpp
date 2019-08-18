﻿
#include "../../LuminoEngine/src/Engine/EngineDomain.hpp"
#include "../../LuminoEngine/src/Asset/AssetManager.hpp"
#include "Project.hpp"
#include "AssetDatabase.hpp"

namespace lna {


//==============================================================================
// AssetDatabase

AssetDatabase::AssetDatabase()
{
}

Ref<ln::AssetModel> AssetDatabase::openAsset(const ln::Path& filePath)
{
    auto assetFile = filePath.str();
    if (!filePath.hasExtension(ln::AssetModel::AssetFileExtension)) {
        assetFile += ln::AssetModel::AssetFileExtension;
    }
    
    if (LN_REQUIRE(ln::FileSystem::existsFile(assetFile))) return nullptr;

    auto asset = ln::makeObject<ln::AssetModel>();
    if (!asset->loadInternal(assetFile)) {
        LN_NOTIMPLEMENTED();    // TODO: error
        return nullptr;
    }

    return asset;
}

ln::Result AssetDatabase::importAsset(const ln::Path& sourceFilePath, const ln::Path& destinationFilePath)
{
    if (sourceFilePath.hasExtension("png")) {
        auto texture = ln::makeObject<ln::Texture2D>(sourceFilePath);
        texture->setAssetId(ln::Uuid::generate());
        texture->setAssetSource(destinationFilePath.fileName());

        auto asset = ln::makeObject<ln::AssetModel>(texture);
        asset->saveInternal(destinationFilePath.str() + ln::AssetModel::AssetFileExtension);

        if (ln::Path::compare(sourceFilePath, destinationFilePath) != 0)
            ln::FileSystem::copyFile(sourceFilePath, destinationFilePath, ln::FileCopyOption::Overwrite);

        return true;
    }
    return false;
}

ln::Result AssetDatabase::createAsset(ln::Object* asset, const ln::Path& filePath)
{
    auto t = ln::AssetModel::create(asset);
    asset->setAssetId(ln::Uuid::generate());
    ln::String json = ln::JsonSerializer::serialize(*t, ln::JsonFormatting::Indented);
    ln::FileSystem::writeAllText(filePath, json);
    return true;
}

bool AssetDatabase::isAssetFile(const ln::Path& file)
{
    return file.hasExtension(ln::AssetModel::AssetFileExtension);
}

bool AssetDatabase::isImportedAssetFile(const ln::Path& file)
{
    return ln::FileSystem::existsFile(file.str() + ln::AssetModel::AssetFileExtension);
}

ln::Result AssetDatabase::init(Project* owner)
{
    ln::detail::EngineDomain::assetManager()->buildAssetIndexFromLocalFiles(owner->assetsDir());

    return true;
}

} // namespace lna
