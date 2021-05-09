#include "pch.h"
#pragma hdrstop
#include "cube.h"

using namespace quadric;

Cube::Cube(CommandBuffer cmdBuffer, Allocator allocator /* nullptr */):
    Quadric(24, 36, std::move(cmdBuffer), std::move(allocator))
{
    constexpr float verts[] = {
        -1, -1, 1, -1, 0, 0, 0, 0,
        -1, 1, 1, -1, 0, 0, 0, 1,
        -1, -1, -1, -1, 0, 0, 1, 0,
        -1, 1, -1, -1, 0, 0, 1, 1,
        1, -1, -1, 1, 0, 0, 0, 0,
        1, 1, -1, 1, 0, 0, 0, 1,
        1, -1, 1, 1, 0, 0, 1, 0,
        1, 1, 1, 1, 0, 0, 1, 1,
        -1, -1, 1, 0, -1, 0, 0, 0,
        -1, -1, -1, 0, -1, 0, 0, 1,
        1, -1, 1, 0, -1, 0, 1, 0,
        1, -1, -1, 0, -1, 0, 1, 1,
        -1, 1, -1, 0, 1, 0, 0, 0,
        -1, 1, 1, 0, 1, 0, 0, 1,
        1, 1, -1, 0, 1, 0, 1, 0,
        1, 1, 1, 0, 1, 0, 1, 1,
        -1, -1, -1, 0, 0, -1, 0, 0,
        -1, 1, -1, 0, 0, -1, 0, 1,
        1, -1, -1, 0, 0, -1, 1, 0,
        1, 1, -1, 0, 0, -1, 1, 1,
        1, -1, 1, 0, 0, 1, 0, 0,
        1, 1, 1, 0, 0, 1, 0, 1,
        -1, -1, 1, 0, 0, 1, 1, 0,
        -1, 1, 1, 0, 0, 1, 1, 1,
    };
    constexpr uint16_t faces[] = {
        0, 1, 2, 2, 1, 3,
        4, 5, 6, 6, 5, 7,
        8, 9, 10, 10, 9, 11,
        12, 13, 14, 14, 13, 15,
        16, 17, 18, 18, 17, 19,
        20, 21, 22, 22, 21, 23
    };
    memcpy(mesh->mapVertices(), verts, sizeof(verts));
    memcpy(mesh->mapIndices(), faces, sizeof(faces));
    mesh->unmap();
}
