#include "pch.h"
#pragma hdrstop
#include "quadric.h"

using namespace quadric;

Quadric::Quadric(uint16_t numVertices, uint32_t numFaces, CommandBuffer cmdBuffer):
    numVertices(numVertices),
    numFaces((uint16_t)numFaces),
    mesh(newMesh(numVertices, numFaces, std::move(cmdBuffer)))
{
    if (numFaces > std::numeric_limits<uint16_t>::max())
        throw std::length_error("face count exceed maximum unsigned short value");
}

Quadric::~Quadric() {}

const VertexInput& Quadric::getVertexInput() const noexcept
{
    return mesh->getVertexInput();
}

void Quadric::draw(CommandBuffer cmdBuffer) const noexcept
{
    mesh->bind(cmdBuffer);
    mesh->draw(std::move(cmdBuffer), numFaces * 3, 0);
}
