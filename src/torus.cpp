#include "pch.h"
#pragma hdrstop
#include "torus.h"
#include "sincosTable.h"

using namespace quadric;
using namespace rapid;
using namespace rapid::constants;

Torus::Torus(float innerRadius, float outerRadius, uint16_t sides, uint16_t rings,
    bool textureMirrorRepeat, std::shared_ptr<magma::CommandBuffer> cmdBuffer):
    Quadric((sides + 1) * (rings + 1), sides * rings * 2, cmdBuffer->getDevice()),
    innerRadius(innerRadius),
    outerRadius(outerRadius),
    sides(sides),
    rings(rings)
{
    // theta - angle on xy plane
    const float thetaStep = twoPi/rings;
    SinCosTable theta(0.f, thetaStep, rings + 1);
    // phi = angle on xz plane
    const float phiStep = twoPi/sides;
    SinCosTable phi(0.f, phiStep, sides + 1);
    // Calculate first ring
    Vertex *verts = vertices->getMemory()->map<Vertex>();
    for (uint16_t side = 0; side < sides + 1; ++side)
    {
        Vertex& v = verts[side];
        v.pos.x = innerRadius * phi.cos(side) + outerRadius;
        v.pos.z = innerRadius * phi.sin(side);
        v.pos.y = 0.f;
        v.normal.x = phi.cos(side);
        v.normal.z = phi.sin(side);
        v.normal.y = 0.f;
        v.tex.s = 0.f;
        v.tex.t = side/(float)sides;
    }
    // Rotate first ring to get other rings
    for (uint16_t ring = 1; ring < rings + 1; ++ring)
    {
        const float s = calcSCoord(ring, textureMirrorRepeat);
        for (uint16_t side = 0; side < sides + 1; ++side)
        {
            const Vertex& ref = verts[side];
            const uint16_t vertexIndex = calcVertexIndex(ring, side);
            Vertex& v = verts[vertexIndex];
            v.pos.x = ref.pos.x * theta.cos(ring);
            v.pos.y = -ref.pos.x * theta.sin(ring);
            v.pos.z = ref.pos.z;
            v.normal.x = ref.normal.x * theta.cos(ring);
            v.normal.y = -ref.normal.x * theta.sin(ring);
            v.normal.z = ref.normal.z;
            v.tex.s = s;
            v.tex.t = ref.tex.t;
        }
    }
    // Build indices
    Face *faces = indices->getMemory()->map<Face>();
    for (uint16_t ring = 0; ring < rings; ++ring)
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

inline float Torus::calcSCoord(uint16_t ring, bool mirror) const noexcept
{
    const uint16_t count = mirror ? (rings >> 1) : rings;
    const float coordS = ring/(float)count;
    return (coordS <= 1.f) ? coordS : 2.f - coordS;
}

inline uint16_t Torus::calcVertexIndex(uint16_t ring, uint16_t side) const noexcept
{
    const uint32_t index = ring * (sides + 1) + side;
    assert(index <= std::numeric_limits<uint16_t>::max());
    return (uint16_t)index;
}
