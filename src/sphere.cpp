#include "pch.h"
#pragma hdrstop
#include "sphere.h"
#include "sincosTable.h"

using namespace quadric;
using namespace rapid;
using namespace rapid::constants;

Sphere::Sphere(float radius, uint16_t slices, uint16_t stacks, bool spherical,
    std::shared_ptr<magma::CommandBuffer> cmdBuffer):
    Quadric(2 + slices * (stacks - 1), 2 * slices + (stacks - 2) * (2 * slices), cmdBuffer->getDevice()),
    radius(radius),
    slices(slices),
    stacks(stacks)
{
    if (radius < 0.f)
        throw std::invalid_argument("invalid sphere radius");
    if ((slices < 3) || (stacks < 3))
        throw std::invalid_argument("invalid sphere parameter");
    // phi - angle on xz plane
    const float phiStep = twoPi/slices;
    const float phiStart = halfPi;
    const SinCosTable phi(phiStart, phiStep, slices);
    const float step = pi/stacks;
    float theta = step;

    Vertex *v = mapVertices();
    Face *face = mapIndices();

    // North pole
    pole(radius, spherical, v++);
    for (uint16_t stack = 0; stack < stacks - 1; ++stack)
    {
        float sinTheta, cosTheta;
        sincosEst(&sinTheta, &cosTheta, theta);
        for (uint16_t slice = 0; slice < slices; ++slice)
        {
            v->pos.x = radius * phi.cos(slice);
            v->pos.z = radius * phi.sin(slice);
            v->pos.y = radius * cosTheta;
            v->normal.x = sinTheta * phi.cos(slice);
            v->normal.z = sinTheta * phi.sin(slice);
            v->normal.y = cosTheta;
            if (spherical)
            {   // https://www.mvps.org/directx/articles/spheremap.htm
                v->tex.s = asinf(v->normal.x) * invPi + .5f;
                v->tex.t = asinf(v->normal.y) * invPi + .5f;
            }
            else
            {   // https://people.cs.clemson.edu/~dhouse/courses/405/notes/texture-maps.pdf
                const float theta = atan2f(v->normal.z, v->normal.x);
                const float phi = acosf(-v->normal.y);
                v->tex.s = (theta + pi) * invTwoPi;
                v->tex.t = phi * invPi;
            }
            ++v;
            if (slice > 0)
            {
                if (0 == stack)
                {   // Top stack is triangle fan
                    face->v[0] = 0;
                    face->v[1] = slice + 1;
                    face->v[2] = slice;
                    ++face;
                }
                else
                {   // Stacks in between top and bottom are quad strips
                    face->v[0] = calcVertexIndex(slice - 1, stack - 1);
                    face->v[1] = calcVertexIndex(slice, stack - 1);
                    face->v[2] = calcVertexIndex(slice - 1, stack);
                    ++face;
                    face->v[0] = calcVertexIndex(slice, stack - 1);
                    face->v[1] = calcVertexIndex(slice, stack);
                    face->v[2] = calcVertexIndex(slice - 1, stack);
                    ++face;
                }
            }
        }
        theta += step;
        if (0 == stack)
        {
            face->v[0] = 0;
            face->v[1] = 1;
            face->v[2] = slices;
            ++face;
        }
        else
        {
            face->v[0] = calcVertexIndex(slices - 1, stack - 1);
            face->v[1] = calcVertexIndex(0, stack - 1);
            face->v[2] = calcVertexIndex(slices - 1, stack);
            ++face;
            face->v[0] = calcVertexIndex(0, stack - 1);
            face->v[1] = calcVertexIndex(0, stack);
            face->v[2] = calcVertexIndex(slices - 1, stack);
            ++face;
        }
    }
    // South pole
    pole(-radius, spherical, v);
    // Bottom stack is triangle fan
    uint16_t bottomStack = stacks - 2;
    for (uint16_t slice = 1; slice < slices; ++slice)
    {
        face->v[0] = calcVertexIndex(slice - 1, bottomStack);
        face->v[1] = calcVertexIndex(slice, bottomStack);
        face->v[2] = numVertices - 1;
        ++face;
    }
    face->v[0] = calcVertexIndex(slices - 1, bottomStack);
    face->v[1] = calcVertexIndex(0, bottomStack);
    face->v[2] = numVertices - 1;

    upload(std::move(cmdBuffer));
}

void Sphere::pole(float radius, bool spherical, Vertex *v) const noexcept
{
    v->pos.x = 0.f;
    v->pos.y = radius;
    v->pos.z = 0.f;
    v->normal.x = 0.f;
    v->normal.y = sign(radius);
    v->normal.z = 0.f;
    if (spherical)
    {   // https://www.mvps.org/directx/articles/spheremap.htm
        v->tex.s = asinf(v->normal.x) * invPi + .5f;
        v->tex.t = asinf(v->normal.y) * invPi + .5f;
    }
    else
    {   // https://people.cs.clemson.edu/~dhouse/courses/405/notes/texture-maps.pdf
        const float theta = atan2f(v->normal.z, v->normal.x);
        const float phi = acosf(-v->normal.y);
        v->tex.s = (theta + pi) * invTwoPi;
        v->tex.t = phi * invPi;
    }
}

inline uint16_t Sphere::calcVertexIndex(uint16_t slice, uint16_t stack) const noexcept
{
    const uint32_t index = stack * slices + slice + 1;
    assert(index < std::numeric_limits<uint16_t>::max());
    return (uint16_t)index;
}
