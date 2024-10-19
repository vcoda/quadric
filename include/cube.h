#pragma once
#include "quadric.h"

namespace quadric
{
    class Cube : public Quadric
    {
    public:
        explicit Cube(const CommandBuffer& cmdBuffer, Allocator allocator = nullptr);
    };
}
