#pragma once
#include <vector>
#include "quadric.h"

namespace quadric
{
    // https://www.scratchapixel.com/lessons/advanced-rendering/bezier-curve-rendering-utah-teapot

    class BezierPatch : public Quadric
    {
    public:
        explicit BezierPatch(const uint32_t patches[][16], const uint16_t numPatches, const float patchVertices[][3],
            uint16_t divs, const CommandBuffer& cmdBuffer, Allocator allocator = nullptr);
        uint16_t getNumPatches() const noexcept { return numPatches; }
        uint16_t getNumPatchVertices() const noexcept { return numPatchVertices; }
        void draw(const CommandBuffer& cmdBuffer) const noexcept override;
        void drawInstanced(const CommandBuffer& cmdBuffer, uint32_t instanceCount) const noexcept;

    private:
        const uint16_t numPatches;
        const uint16_t numPatchVertices;
    };
}
