#include "pch.h"
#pragma hdrstop
#include "bezierPatch.h"
namespace {
#include "bezier.inl"
}

using namespace quadric;
using namespace rapid;
using namespace rapid::constants;

BezierPatch::BezierPatch(const uint32_t patches[][16], uint16_t numPatches, const float patchVertices[][3],
    uint16_t divs, CommandBuffer cmdBuffer, Allocator allocator /* nullptr */):
    Quadric(numPatches * (divs + 1) * (divs + 1), divs * divs * 2, std::move(cmdBuffer), std::move(allocator)),
    numPatches(numPatches),
    numPatchVertices((divs + 1) * (divs + 1))
{
    if (numPatches < 1)
        throw std::invalid_argument("invalid bezier patch count");
    if (divs < 1)
        throw std::invalid_argument("invalid bezier subdivision parameter");
    Vertex *v = mesh->mapVertices();
    vector3 controlPoints[16];
    for (uint16_t np = 0; np < numPatches; ++np)
    {   // Set patch control points
        for (uint16_t i = 0; i < 16; ++i)
        {
            controlPoints[i] = vector3(patchVertices[patches[np][i] - 1][0],
                                       patchVertices[patches[np][i] - 1][1],
                                       patchVertices[patches[np][i] - 1][2]);
        }
        // Generate grid
        for (uint16_t j = 0, k = 0; j <= divs; ++j)
        {
            const float t = j/(float)divs;
            for (uint16_t i = 0; i <= divs; ++i, ++k)
            {
                const float s = i/(float)divs;
                const vector3 pos = evalBezierPatch(controlPoints, s, t);
                const vector3 dU = dUBezier(controlPoints, s, t);
                const vector3 dV = dVBezier(controlPoints, s, t);
                const vector3 normal = (dU ^ dV).normalized();
                pos.store(&v->pos);
                std::swap(v->pos.y, v->pos.z); // swap Y,Z
                normal.store(&v->normal);
                std::swap(v->normal.y, v->normal.z); // swap Y,Z
                v->tex.s = 1.f - s;
                v->tex.t = 1.f - t;
                ++v;
            }
        }
    }
    const uint16_t numPatchFaces = divs * divs;
    std::vector<uint16_t> patchIndices(numPatchFaces * 4);
    // All patches are subdivided in the same way, so here we share the same topology
    for (uint16_t j = 0, k = 0; j < divs; ++j)
    {
        for (uint16_t i = 0; i < divs; ++i, ++k)
        {
            patchIndices[k * 4] = (divs + 1) * j + i;
            patchIndices[k * 4 + 1] = (divs + 1) * j + i + 1;
            patchIndices[k * 4 + 2] = (divs + 1) * (j + 1) + i + 1;
            patchIndices[k * 4 + 3] = (divs + 1) * (j + 1) + i;
        }
    }
    Face *faces = mesh->mapIndices();
    for (uint16_t i = 0, k = 0; i < numPatchFaces; ++i, k += 4)
    {
        const uint16_t faceIndex = i * 2;
        Face& face = faces[faceIndex];
        face.v[0] = patchIndices[k];
        face.v[1] = patchIndices[k + 2];
        face.v[2] = patchIndices[k + 1];
        Face& face1 = faces[faceIndex + 1];
        face1.v[0] = patchIndices[k];
        face1.v[1] = patchIndices[k + 3];
        face1.v[2] = patchIndices[k + 2];
    }
    mesh->unmap();
}

void BezierPatch::draw(CommandBuffer cmdBuffer) const noexcept
{
    mesh->bind(cmdBuffer);
    for (uint16_t i = 0; i < numPatches; ++i)
    {
        const uint32_t baseVertex = numPatchVertices * i;
        mesh->draw(cmdBuffer, numFaces * 3, (int)baseVertex);
    }
}
