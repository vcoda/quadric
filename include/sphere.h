#pragma once
#include "quadric.h"

namespace quadric
{
    class Sphere : public Quadric
    {
    public:
        explicit Sphere(float radius, uint16_t slices, uint16_t stacks, bool spherical,
            CommandBuffer cmdBuffer, Allocator allocator = nullptr);
        float getRadius() const noexcept { return radius; }
        uint16_t getNumSlices() const noexcept { return slices; }
        uint16_t getNumStacks() const noexcept { return stacks; }

    private:
        void pole(float radius, bool spherical, Vertex *v) const noexcept;
        uint16_t calcVertexIndex(uint16_t slice, uint16_t stack) const noexcept;

        const float radius;
        const uint16_t slices;
        const uint16_t stacks;
    };
}
