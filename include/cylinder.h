#pragma once
#include "quadric.h"

namespace quadric
{
    class SinCosTable;

    class Cylinder : public Quadric
    {
    public:
        explicit Cylinder(float topRadius, float bottomRadius, float length, uint16_t sides, uint16_t rings, bool capped,
            bool mirrorTexture, std::shared_ptr<magma::CommandBuffer> cmdBuffer);
        float getTopRadius() const noexcept { return topRadius; }
        float getBottomRadius() const noexcept { return bottomRadius; }
        uint16_t getNumSides() const noexcept { return sides; }
        uint16_t getNumRings() const noexcept { return rings; }
        bool isCapped() const noexcept { return capped; }

    private:
        void pole(float y, Vertex& v) const noexcept;
        void cap(const SinCosTable& phi, float radius, float y,
            Vertex *verts, Face *faces, uint16_t firstFaceIndex) const noexcept;
        float calcSCoord(uint16_t side, bool mirror) const noexcept;
        float calcTCoord(uint16_t ring) const noexcept;
        uint16_t calcVertexIndex(uint16_t ring, uint16_t side) const noexcept;
        uint16_t calcPoleVertexIndex(bool south) const noexcept;

        const float topRadius;
        const float bottomRadius;
        const uint16_t sides;
        const uint16_t rings;
        const bool capped;
    };
}
