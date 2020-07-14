#pragma once
#include <cstdint>
#include <vector>

namespace quadric
{
    class SinCosTable
    {
    public:
        explicit SinCosTable(float start, float step, uint16_t count);
        inline float sin(uint16_t i) const noexcept
            { return lut[i/4].qsin[i%4]; }
        inline float cos(uint16_t i) const noexcept
            { return lut[i/4].qcos[i%4]; }

    private:
        struct alignas(16) VectorSinCos
        {
            float qsin[4];
            float qcos[4];
        };

        std::vector<VectorSinCos> lut;
    };
}
