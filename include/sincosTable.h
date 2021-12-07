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
#if defined(__x86_64__) || defined(_M_AMD64)
        typedef rapid::float4a float4;
#else
        typedef rapid::float4 float4;
#endif
        struct alignas(16) QuadSinCos
        {
            union {
                float4 vsin;
                float qsin[4];
            };
            union {
                float4 vcos;
                float qcos[4];
            };
        };

        std::vector<QuadSinCos> lut;
    };
}
