#include "pch.h"
#pragma hdrstop
#include "sincosTable.h"

using namespace quadric;
using namespace rapid;

SinCosTable::SinCosTable(float start, float step, uint16_t count):
    lut((count >> 2) + (count%4 ? 1 : 0))
{
    const vector4 steps(step * 4.f);
    vector4 angles(start, start + step, start + step * 2.f, start + step * 3.f);
    for (auto& it : lut)
    {
        vector4 vsin, vcos;
        sincosEst(&vsin, &vcos, angles);
        vsin.store(&it.vsin);
        vcos.store(&it.vcos);
        angles += steps;
    }
}
