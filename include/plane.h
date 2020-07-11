#pragma once
#include "quadric.h"

namespace quadric
{
    class Plane : public Quadric
    {
    public:
        explicit Plane(float width, float length, bool twoSided,
            std::shared_ptr<magma::CommandBuffer> cmdBuffer);
        float getWidth() const noexcept { return width; }
        float getLength() const noexcept { return length; }

    private:
        const float width;
        const float length;
    };
}
