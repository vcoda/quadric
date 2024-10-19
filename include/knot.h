#pragma once
#include "quadric.h"

namespace quadric
{
    class Knot : public Quadric
    {
    public:
        explicit Knot(float radius, uint16_t turns, uint16_t sides, uint16_t rings,
            bool mirrorTexture, const CommandBuffer& cmdBuffer, Allocator allocator = nullptr);
        float getRadius() const noexcept { return radius; }
        uint16_t getNumTurns() const noexcept { return turns; }
        uint16_t getNumSides() const noexcept { return sides; }
        uint16_t getNumRings() const noexcept { return rings; }

    private:
        inline float calcSCoord(uint16_t ring, bool mirror) const noexcept;
        inline uint16_t calcVertexIndex(uint16_t ring, uint16_t side) const noexcept;

        const float radius;
        const uint16_t turns;
        const uint16_t sides;
        const uint16_t rings;
    };
}
