/*
Collection of quadric surfaces to draw with Vulkan.
Copyright (C) 2020 Victor Coda.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#pragma once
#include <cstdint>
#include <memory>
#include <exception>
#if !defined(DIRECTX_MATH_VERSION)
// Include once
#include "rapid/rapid.h"
#endif

namespace magma
{
    class Device;
    class VertexBuffer;
    class IndexBuffer;
    class SrcTransferBuffer;
    class CommandBuffer;
    class VertexInputState;
}

namespace quadric
{
    struct TexCoord
    {
        float s, t;
    };

    struct Vertex
    {
        rapid::float3 pos;
        rapid::float3 normal;
        TexCoord tex;
    };

    struct Face
    {
        uint16_t v[3];
    };
}
