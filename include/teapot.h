#pragma once
#include "bezierPatch.h"

namespace quadric
{
    class Teapot : public BezierPatch
    {
    public:
        explicit Teapot(uint16_t subdivisionDegree,
            std::shared_ptr<magma::CommandBuffer> cmdBuffer);
    };
}
