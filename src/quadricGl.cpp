#include "pch.h"
#pragma hdrstop
#ifdef QUADRIC_OPENGL
#include "quadric.h"

using namespace quadric;

Quadric::Quadric(uint16_t numVertices, uint32_t numFaces, Device device):
    numVertices(numVertices),
    numFaces((uint16_t)numFaces)
{
}

Quadric::~Quadric()
{
}

const VertexInputState& Quadric::getVertexInput() const noexcept
{
}

void Quadric::draw(CommandBuffer cmdBuffer) const noexcept
{
}

void Quadric::upload(CommandBuffer cmdBuffer)
{
}
#endif // QUADRIC_OPENGL
