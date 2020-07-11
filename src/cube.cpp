#include "pch.h"
#pragma hdrstop
#include "cube.h"

using namespace quadric;

Cube::Cube(std::shared_ptr<magma::CommandBuffer> cmdBuffer):
    Quadric(24, 36, cmdBuffer->getDevice())
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
    memcpy(vertices->getMemory()->map(), verts, sizeof(verts));
    memcpy(indices->getMemory()->map(), faces, sizeof(faces));
    upload(std::move(cmdBuffer));
}
