#include "pch.h"
#pragma hdrstop
#include "knot.h"
#include "sincosTable.h"

using namespace quadric;
using namespace rapid;
using namespace rapid::constants;

Knot::Knot(float radius, uint16_t turns, uint16_t sides, uint16_t rings,
    bool mirrorTexture, std::shared_ptr<magma::CommandBuffer> cmdBuffer):
    Quadric((sides + 1) * (rings + 1), sides * rings * 2, cmdBuffer->getDevice()),
    turns(turns),
    sides(sides),
    rings(rings),
    radius(radius)
{
    if (radius < 0.f)
        throw std::invalid_argument("invalid knot radius");
    if (turns%2 == 0) // Should be odd
        throw std::invalid_argument("not an knot turn count");
    if (sides < 3 || rings < turns * 2)
        throw std::invalid_argument("invalid knot parameter");
    const float step = twoPi/rings;
    SinCosTable theta(0.f, turns * step, rings + 1); // xy plane
    SinCosTable phi(0.f, 2.f * step, rings + 1); // xz plane
    // Calculate center of each ring
    std::vector<vector3> ringCenters(rings + 1);
    for (uint16_t ring = 0; ring < rings + 1; ++ring)
    {
        const float f = 1.f + .3f * theta.cos(ring);
        ringCenters[ring] = vector3(
            f * phi.cos(ring),
            f * .3f * theta.sin(ring),
            f * phi.sin(ring));
    }
    Vertex *verts = vertices->getMemory()->map<Vertex>();
    for (uint16_t ring = 0; ring < rings + 1; ++ring)
    {
        const float s = calcSCoord(ring, mirrorTexture);
        for (uint16_t side = 0; side < sides + 1; ++side)
        {   // Get the vector from the center of this ring to the center of the next
            vector3 tangent;
            if (ring < rings)
                tangent = ringCenters[ring + 1] - ringCenters[ring];
            else
                tangent = ringCenters[1] - ringCenters[ring];
            tangent.normalize();
            // Calculate the vector perpendicular to the tangent,
            // pointing approximately in the positive Y direction
            const vector3 up(0.f, 1.f, 0.f);
            const vector3 bitangent = up ^ tangent;
            const vector3 normal = (tangent ^ bitangent).normalized() * radius;
            // Rotate about tangent vector to form the ring
            const float angle = radians(side * 360.f/sides);
            const matrix rot = rotationNormal(tangent, angle);
            const vector3 pos = ringCenters[ring] + rot * normal;
            const uint16_t vertexIndex = calcVertexIndex(ring, side);
            Vertex& v = verts[vertexIndex];
            pos.store(&v.pos);
            (pos - ringCenters[ring]).normalized().store(&v.normal);
            v.tex.s = s;
            v.tex.t = side/(float)(sides);
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

inline float Knot::calcSCoord(uint16_t ring, bool mirror) const noexcept
{
    const uint16_t count = mirror ? (rings >> 1) : rings;
    const float coordS = ring/(float)count;
    return (coordS <= 1.f) ? coordS : 2.f - coordS;
}

uint16_t Knot::calcVertexIndex(uint16_t ring, uint16_t side) const noexcept
{
    const uint32_t index = ring * (sides + 1) + side;
    assert(index <= std::numeric_limits<uint16_t>::max());
    return (uint16_t)index;
}
