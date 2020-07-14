#include "pch.h"
#pragma hdrstop
#include "sincosTable.h"

using namespace quadric;
using namespace rapid;

SinCosTable::SinCosTable(float start, float step, uint16_t count):
    sines(count),
    cosines(count)
{
    vector4 steps(0.f, step, step * 2.f, step * 3.f);
    vector4 angles(vector4(start) + steps);
    vector4 adds(step * 4.f);
    vector4 sinv, cosv;
    for (uint16_t i = 0, j = count/4; i < j; ++i)
    {
        sincosEst(&sinv, &cosv, angles);
        sinv.store((float4 *)&sines[i << 2]);
        cosv.store((float4 *)&cosines[i << 2]);
        angles += adds;
    }
    const uint16_t remainder = count%4;
    if (remainder)
    {
        for (uint16_t i = count - remainder; i < count; ++i)
        {
            float angle = start + i * step;
            sincosEst(&sines[i], &cosines[i], angle);
        }
    }
}
