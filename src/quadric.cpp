#include "pch.h"
#pragma hdrstop
#include "quadric.h"

using namespace quadric;

Quadric::Quadric(uint16_t numVertices, uint32_t numFaces, const CommandBuffer& cmdBuffer, Allocator allocator):
    numVertices(numVertices),
    numFaces((uint16_t)numFaces),
    mesh(newMesh(numVertices, numFaces, cmdBuffer, std::move(allocator)))
{
    if (numFaces > std::numeric_limits<uint16_t>::max())
        throw std::length_error("face count exceed maximum unsigned short value");
}

Quadric::~Quadric() {}

const VertexInput& Quadric::getVertexInput() const noexcept
{
    return mesh->getVertexInput();
}

const BoundingBox& Quadric::getBoundingBox() const noexcept
{
    return mesh->getBoundingBox();
}

void Quadric::draw(const CommandBuffer& cmdBuffer) const noexcept
{
    mesh->bind(cmdBuffer);
    mesh->draw(cmdBuffer, numFaces * 3, 0);
}

void Quadric::drawInstanced(const CommandBuffer& cmdBuffer, uint32_t instanceCount) const noexcept
{
    mesh->bind(cmdBuffer);
    mesh->drawInstanced(cmdBuffer, numFaces * 3, instanceCount, 0);
}
