#pragma once
#include "quadric.h"

namespace quadric
{
    class Plane : public Quadric
    {
    public:
        explicit Plane(float width, float length, bool twoSided,
            CommandBuffer cmdBuffer, Allocator allocator = nullptr);
        float getWidth() const noexcept { return width; }
        float getLength() const noexcept { return length; }

    private:
        const float width;
        const float length;
    };
}
