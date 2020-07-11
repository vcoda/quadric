#include "pch.h"
#pragma hdrstop
#include "cylinder.h"
#include "sincosTable.h"

using namespace quadric;
using namespace rapid;
using namespace rapid::constants;

Cylinder::Cylinder(float topRadius, float bottomRadius, float length, uint16_t sides, uint16_t rings,
    bool textureMirrorRepeat, std::shared_ptr<magma::CommandBuffer> cmdBuffer):
    Quadric((sides + 1) * rings, sides * (rings - 1) * 2, cmdBuffer->getDevice()),
    topRadius(topRadius),
    bottomRadius(bottomRadius),
    sides(sides),
    rings(rings)
{
    const float radiusDelta = topRadius - bottomRadius;
    const float radiusStep = radiusDelta/rings;
    const float yStep = length/rings;
    float radius = topRadius;
    float y = length/2.f;
    float ny = -radiusDelta/length;
#ifdef _WIN32
    if (_isnan(ny))
        ny = 0.f;
#else
    if (isnan(ny))
        ny = 0.f;
#endif
    // phi - angle on xz plane
    const float phiStep = rapid::constants::twoPi/sides;
    SinCosTable phi(rapid::constants::halfPi, phiStep, sides + 1);
    // Calculate rings
    Vertex *verts = vertices->getMemory()->map<Vertex>();
    for (uint16_t ring = 0; ring < rings; ++ring)
    {
        for (uint16_t side = 0; side < sides + 1; ++side)
        {
            const uint16_t vertexIndex = calcVertexIndex(ring, side);
            Vertex& v = verts[vertexIndex];
            v.pos.x = radius * phi.cos(side);
            v.pos.z = radius * phi.sin(side);
            v.pos.y = y;
            v.normal.x = phi.cos(side);
            v.normal.z = phi.sin(side);
            v.normal.y = ny;
            vector3 normal(v.normal);
            normal.normalize();
            normal.store(&v.normal);
            v.tex.s = calcSCoord(side, textureMirrorRepeat);
            v.tex.t = calcTCoord(ring);
        }
        y -= yStep;
        radius -= radiusStep;
    }
    // Build indices
    Face *faces = indices->getMemory()->map<Face>();
    for (uint16_t ring = 0; ring < rings - 1; ++ring)
    {
        for (uint16_t side = 0; side < sides; ++side)
        {
            const uint16_t faceIndex = (ring * sides + side) * 2;
            Face& face = faces[faceIndex];
            face.v[0] = calcVertexIndex(ring, side);
            face.v[1] = calcVertexIndex(ring, side + 1);
            face.v[2] = calcVertexIndex(ring + 1, side);
            Face& face1 = faces[faceIndex + 1];
            face1.v[0] = calcVertexIndex(ring, side + 1);
            face1.v[1] = calcVertexIndex(ring + 1, side + 1);
            face1.v[2] = calcVertexIndex(ring + 1, side);
        }
    }
    upload(std::move(cmdBuffer));
}

inline float Cylinder::calcSCoord(uint16_t side, bool mirror) const noexcept
{
    const uint16_t count = mirror ? (sides >> 1) : sides;
    const float coordS = side/(float)count;
    return (coordS <= 1.f) ? coordS : 2.f - coordS;
}

inline float Cylinder::calcTCoord(uint16_t ring) const noexcept
{
    return 1.f - ring/(float)(rings - 1);
}

inline uint16_t Cylinder::calcVertexIndex(uint16_t ring, uint16_t side) const noexcept
{
    const uint32_t index = ring * (sides + 1) + side;
    assert(index <= std::numeric_limits<uint16_t>::max());
    return (uint16_t)index;
}
