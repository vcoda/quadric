#pragma once
#include "quadric.h"

namespace quadric
{
    class Cube : public Quadric
    {
    public:
        explicit Cube(std::shared_ptr<magma::CommandBuffer> cmdBuffer);
    };
}
