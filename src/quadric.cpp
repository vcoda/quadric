#include "pch.h"
#pragma hdrstop
#if !defined(QUADRIC_VULKAN) && !defined(QUADRIC_OPENGL) && !defined(QUADRIC_DIRECT3D)
#include "quadric.h"
#include "../../magma/states/vertexInputStructure.h"

using namespace quadric;

Quadric::Quadric(uint16_t numVertices, uint32_t numFaces, backend::Device device):
    numVertices(numVertices),
    numFaces((uint16_t)numFaces),
    vertices(std::make_shared<magma::SrcTransferBuffer>(device, numVertices * sizeof(Vertex))),
    indices(std::make_shared<magma::SrcTransferBuffer>(std::move(device), numFaces * sizeof(Face)))
{
    if (numFaces > std::numeric_limits<uint16_t>::max())
        throw std::length_error("face count exceed maximum unsigned short value");
}

Quadric::~Quadric()
{
}

const backend::VertexInputState& Quadric::getVertexInput() const noexcept
{
    static magma::VertexInputStructure<Vertex> vertexInput(0, {
        {0, &Vertex::pos},
        {1, &Vertex::normal},
        {2, &Vertex::tex}});
    return vertexInput;
}

void Quadric::draw(backend::CommandBuffer cmdBuffer) const noexcept
{
    cmdBuffer->bindVertexBuffer(0, vertexBuffer);
    cmdBuffer->bindIndexBuffer(indexBuffer);
    cmdBuffer->drawIndexed(indexBuffer->getIndexCount());
}

Vertex *Quadric::mapVertices()
{
    return (Vertex *)vertices->getMemory()->map();
}

Face *Quadric::mapIndices()
{
    return (Face *)indices->getMemory()->map();
}

void Quadric::upload(backend::CommandBuffer cmdBuffer)
{
    vertices->getMemory()->unmap();
    indices->getMemory()->unmap();
    vertexBuffer = std::make_shared<magma::VertexBuffer>(cmdBuffer, std::move(vertices));
    indexBuffer = std::make_shared<magma::IndexBuffer>(std::move(cmdBuffer),
        std::move(indices), VK_INDEX_TYPE_UINT16);
}

namespace magma
{
    namespace specialization
    {
        template<> struct VertexAttribute<rapid::float3> : public AttributeFormat<VK_FORMAT_R32G32B32_SFLOAT> {};
        template<> struct VertexAttribute<TexCoord> : public AttributeFormat<VK_FORMAT_R32G32_SFLOAT> {};
    } // namespace specialization
} // namespace magma
#endif // !QUADRIC_VULKAN && !QUADRIC_OPENGL && !QUADRIC_DIRECT3D
