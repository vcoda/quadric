#include "pch.h"
#pragma hdrstop
#include "sincosTable.h"

using namespace quadric;
using namespace rapid;

SinCosTable::SinCosTable(float start, float step, uint16_t count):
    lut((count >> 2) + (count%4 ? 1 : 0))
{
#if defined(__x86_64__) || defined(_M_AMD64)
    struct SinCosFloat4a
    {
        float4a qsin, qcos;
    } *data = (SinCosFloat4a *)lut.data();
#else
    struct SinCosFloat4
    {
        float4 qsin, qcos;
    } *data = (SinCosFloat4 *)lut.data();
#endif
    const vector4 steps(step * 4.f);
    vector4 angles(start,
        start + step,
        start + step * 2.f,
        start + step * 3.f);
    vector4 sinv, cosv;
    for (std::size_t i = 0, n = lut.size();
        i < n;
        ++i, angles += steps, ++data)
    {
        sincosEst(&sinv, &cosv, angles);
        sinv.store(&data->qsin);
        cosv.store(&data->qcos);
    }
}
