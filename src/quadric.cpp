#include "pch.h"
#pragma hdrstop
#include "quadric.h"
#include "../../magma/states/vertexInputStructure.h"

using namespace quadric;
using namespace magma;

Quadric::Quadric(uint16_t numVertices, uint32_t numFaces, std::shared_ptr<Device> device):
    numVertices(numVertices),
    numFaces((uint16_t)numFaces),
    vertices(std::make_shared<SrcTransferBuffer>(device, numVertices * sizeof(Vertex))),
    indices(std::make_shared<SrcTransferBuffer>(std::move(device), numFaces * 3 * sizeof(uint16_t)))
{
    if (numFaces > std::numeric_limits<uint16_t>::max())
        throw std::length_error("face count exceed maximum unsigned short value");
}

const magma::VertexInputState& Quadric::getVertexInput() const noexcept
{
    static VertexInputStructure<Vertex> vertexInput(0, {
        {0, &Vertex::pos},
        {1, &Vertex::normal},
        {2, &Vertex::tex}});
    return vertexInput;
}

void Quadric::draw(std::shared_ptr<CommandBuffer> cmdBuffer) const noexcept
{
    cmdBuffer->bindVertexBuffer(0, vertexBuffer);
    cmdBuffer->bindIndexBuffer(indexBuffer);
    cmdBuffer->drawIndexed(indexBuffer->getIndexCount());
}

void Quadric::upload(std::shared_ptr<CommandBuffer> cmdBuffer)
{
    vertices->getMemory()->unmap();
    indices->getMemory()->unmap();
    vertexBuffer = std::make_shared<VertexBuffer>(cmdBuffer, std::move(vertices));
    indexBuffer = std::make_shared<IndexBuffer>(std::move(cmdBuffer),
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
