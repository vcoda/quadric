#pragma once
#include "quadric.h"

namespace quadric
{
    class Torus : public Quadric
    {
    public:
        explicit Torus(float innerRadius, float outerRadius, uint16_t sides, uint16_t rings,
            bool mirrorTexture, std::shared_ptr<magma::CommandBuffer> cmdBuffer);
        float getInnerRadius() const noexcept { return innerRadius; }
        float getOuterRadius() const noexcept { return outerRadius; }
        uint16_t getNumSides() const noexcept { return sides; }
        uint16_t getNumRings() const noexcept { return rings; }

    private:
        float calcSCoord(uint16_t ring, bool mirror) const noexcept;
        uint16_t calcVertexIndex(uint16_t ring, uint16_t side) const noexcept;

        const float innerRadius;
        const float outerRadius;
        const uint16_t sides;
        const uint16_t rings;
    };
}
