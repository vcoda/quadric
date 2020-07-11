#pragma once
#include <cstdint>
#include <vector>

namespace quadric
{
    class SinCosTable
    {
    public:
        explicit SinCosTable(float start, float step, uint16_t count);
        float sin(uint16_t index) const noexcept { return sines[index]; }
        float cos(uint16_t index) const noexcept { return cosines[index]; }

    private:
        std::vector<float> sines;
        std::vector<float> cosines;
    };
}
