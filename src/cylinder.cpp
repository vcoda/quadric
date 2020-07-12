#include "pch.h"
#pragma hdrstop
#include "cylinder.h"
#include "sincosTable.h"

using namespace quadric;
using namespace rapid;
using namespace rapid::constants;

Cylinder::Cylinder(float topRadius, float bottomRadius, float length, uint16_t sides, uint16_t rings, bool capped,
    bool textureMirrorRepeat, std::shared_ptr<magma::CommandBuffer> cmdBuffer):
    Quadric((sides + 1) * (rings + 1) + (capped ? (sides + 2) * 2 : 0),
        sides * rings * 2 + (capped ? sides * 2 : 0),
        cmdBuffer->getDevice()),
    topRadius(topRadius),
    bottomRadius(bottomRadius),
    sides(sides),
    rings(rings),
    capped(capped)
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
    const float phiStep = twoPi/sides;
    SinCosTable phi(halfPi, phiStep, sides + 1);
    // Calculate rings
    Vertex *verts = vertices->getMemory()->map<Vertex>();
    for (uint16_t ring = 0; ring < rings + 1; ++ring)
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
    if (capped)
    {
        uint16_t firstFaceIndex = sides * rings * 2;
        cap(phi, topRadius, length/2.f, verts, faces, firstFaceIndex);
        firstFaceIndex += sides;
        cap(phi, bottomRadius, -length/2.f, verts, faces, firstFaceIndex);
    }
    upload(std::move(cmdBuffer));
}

void Cylinder::pole(float y, Vertex& v) const noexcept
{
    v.pos.x = 0.f;
    v.pos.y = y;
    v.pos.z = 0.f;
    v.normal = {0.f, sign(y), 0.f};
    v.tex.s = .5f;
    v.tex.t = .5f;
}

void Cylinder::cap(const SinCosTable& phi, float radius, float y,
    Vertex *verts, Face *faces, uint16_t firstFaceIndex) const noexcept
{
    const bool bottom = (y < 0.f);
    const uint16_t poleIndex = calcPoleVertexIndex(bottom);
    pole(y, verts[poleIndex]);
    const float ny = sign(y);
    // Calculate vertices
    uint16_t vertexIndex = poleIndex + 1;
    for (uint16_t side = 0; side < sides + 1; ++side)
    {
        Vertex& v = verts[vertexIndex++];
        v.pos.x = radius * phi.cos(side);
        v.pos.z = radius * phi.sin(side);
        v.pos.y = y;
        v.normal = {0.f, ny, 0.f};
        v.tex.s = phi.cos(side) * .5f + .5f;
        v.tex.t = phi.sin(side) * .5f + .5f;
        if (bottom) // Flip T coord
            v.tex.t = 1.f - v.tex.t;
    }
    // Build indices
    for (uint16_t side = 0; side < sides; ++side)
    {
        Face& face = faces[firstFaceIndex + side];
        face.v[0] = poleIndex + side + 1;
        face.v[1] = poleIndex;
        face.v[2] = poleIndex + side + 2;
        if (bottom) // Swap face indices
            std::swap(face.v[0], face.v[1]);
    }
}

inline float Cylinder::calcSCoord(uint16_t side, bool mirror) const noexcept
{
    const uint16_t count = mirror ? (sides >> 1) : sides;
    const float coordS = side/(float)count;
    return (coordS <= 1.f) ? coordS : 2.f - coordS;
}

inline float Cylinder::calcTCoord(uint16_t ring) const noexcept
{
    return 1.f - ring/(float)rings;
}

inline uint16_t Cylinder::calcVertexIndex(uint16_t ring, uint16_t side) const noexcept
{
    const uint32_t index = ring * (sides + 1) + side;
    assert(index <= std::numeric_limits<uint16_t>::max());
    return (uint16_t)index;
}

uint16_t Cylinder::calcPoleVertexIndex(bool south) const noexcept
{
    uint16_t index = (sides + 1) * (rings + 1);
    if (south) index += (sides + 1) + 1; // top cap
    assert(index <= std::numeric_limits<uint16_t>::max());
    return index;
}
