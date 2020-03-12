﻿
#pragma once
#include "Common.hpp"

namespace ln {

class MeshTilemapLayer
    : public Object
{
    LN_OBJECT;
public:
    void resize(int sizeX, int sizeY, int sizeZ);
    void setTileId(int x, int y, int z, int id);

    int sizeX() const { return m_sizeX; }
    int sizeY() const { return m_sizeY; }
    int sizeZ() const { return m_sizeZ; }

    // 周辺にも適用する
    // autoTilesetId: 0~
    void putAutoTile(int x, int y, int z, int autotileKindId);
    void setAutoTileIdDirect(int x, int y, int z, MeshTileFaceDirection dir, int id);

LN_CONSTRUCT_ACCESS:
    MeshTilemapLayer();
    void init();

private:
    struct Vec3I
    {
        int x, y, z;
    };

    // X, Z 面は Y+ を論理的な上方向とする。
    // Y 面は Z+ を論理的な上方向とする。
    struct AutoTileNearbyInfo
    {
        //PointI points[9];
        int tileIds[9];
        //int topTileIds[9];
    };

    static const Vec3I TopOffsets[6];

    bool isValidIndex(int x, int y, int z) const { return (0 <= x && x < m_sizeX) && (0 <= y && y < m_sizeY) && (0 <= z && z < m_sizeZ); }
    detail::MeshTile& tile(int x, int y, int z) { return m_tiles[((m_sizeX * m_sizeY) * z) + (m_sizeX * y) + x]; }
    const detail::MeshTile& tile(int x, int y, int z) const { return m_tiles[((m_sizeX * m_sizeY) * z) + (m_sizeX * y) + x]; }
    void refreshAutoTile(int x, int y, int z);
    void refreshAutoTileFace(int x, int y, int z, MeshTileFaceDirection dir);
    void makeAutoTileNearbyInfo(int x, int y, int z, MeshTileFaceDirection dir, AutoTileNearbyInfo* outInfo) const;

    void draw(RenderingContext* context, const MeshTileset* tileset);

    List<detail::MeshTile> m_tiles;
    int m_sizeX;
    int m_sizeY;
    int m_sizeZ;

    friend class MeshTilemapModel;
};

} // namespace ln
