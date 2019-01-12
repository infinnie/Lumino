﻿
#include "Internal.hpp"
#include <LuminoEngine/Rendering/Vertex.hpp>
#include "MeshGenerater.hpp"

namespace ln {
namespace detail {

void MeshGeneraterBuffer::setBuffer(Vertex* vertexBuffer, void* indexBuffer, IndexBufferFormat indexFormat, uint32_t indexNumberOffset)
{
    m_vertexBuffer = vertexBuffer;
    m_indexBuffer = indexBuffer;
    m_indexFormat = indexFormat;
    m_indexNumberOffset = indexNumberOffset;
}

void MeshGeneraterBuffer::setV(int index, const Vector3&	position, const Vector2& uv, const Vector3& normal)
{
    m_vertexBuffer[index].position = position;
    m_vertexBuffer[index].uv = uv;
    m_vertexBuffer[index].normal = normal;
    m_vertexBuffer[index].color = m_color;
}

} // namespace detail
} // namespace ln

