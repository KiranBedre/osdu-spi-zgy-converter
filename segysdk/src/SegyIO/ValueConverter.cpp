/*
Copyright 2021 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "ValueConverter.h"

#include <cmath>
#include <limits>

namespace
{
const uint32_t IbmNanMask = 0x61100000;
const uint32_t IbmInf = 0x61100000;
const uint32_t IbmMinusInf = 0xE1100000;
} // namespace

namespace segysdk::segyio
{

ValueConverter::ValueConverter(bool needsSwap)
  : needsSwap(needsSwap)
{
}

void
ValueConverter::Ieee4BytesToFloat(const uint8_t* bytes, float* out, size_t count) const
{
    this->SwapBytes4(bytes, reinterpret_cast<uint32_t*>(out), count);
}

void
ValueConverter::Ieee8BytesToDouble(const uint8_t* bytes, double* out, size_t count) const
{
    this->SwapBytes8(bytes, reinterpret_cast<uint64_t*>(out), count);
}

void
ValueConverter::IBM4BytesToFloat(const uint8_t* bytes, float* out, size_t count) const
{
    this->SwapBytes4(bytes, reinterpret_cast<uint32_t*>(out), count);
    const auto outInts = reinterpret_cast<uint32_t*>(out);
    for (size_t i = 0; i < count; ++i) {
        IBM2Ieee(&outInts[i]);
    }
}

void
ValueConverter::BytesToInt16(const uint8_t* bytes, int16_t* out, size_t count) const
{
    this->SwapBytes2(bytes, reinterpret_cast<uint16_t*>(out), count);
}

void
ValueConverter::BytesToUInt16(const uint8_t* bytes, uint16_t* out, size_t count) const
{
    this->SwapBytes2(bytes, reinterpret_cast<uint16_t*>(out), count);
}

void
ValueConverter::BytesToInt32(const uint8_t* bytes, int32_t* out, size_t count) const
{
    this->SwapBytes4(bytes, reinterpret_cast<uint32_t*>(out), count);
}

void
ValueConverter::BytesToUInt32(const uint8_t* bytes, uint32_t* out, size_t count) const
{
    this->SwapBytes4(bytes, reinterpret_cast<uint32_t*>(out), count);
}

inline void
ValueConverter::IBM2Ieee(uint32_t* fconv)
{
    if (*fconv) {
        { // Special values handling (NaN/ +/-Inf)
            float specialValue;
            const auto spacialInt = reinterpret_cast<uint32_t*>(&specialValue);
            if (*fconv == IbmInf) {
                specialValue = std::numeric_limits<float>::infinity();
                *fconv = *spacialInt;
                return;
            }
            else if (*fconv == IbmMinusInf) {
                specialValue = -std::numeric_limits<float>::infinity();
                *fconv = *spacialInt;
                return;
            }
            else if ((*fconv & IbmNanMask) == IbmNanMask) {
                specialValue = std::numeric_limits<float>::quiet_NaN();
                *fconv = *spacialInt;
                return;
            }
        }

        unsigned fmant = 0x00ffffff & *fconv;
        if (fmant == 0) {
            *fconv = 0;
        }
        else {
            int t = static_cast<int>((0x7f000000 & *fconv) >> 22) - 130;
            while (!(fmant & 0x00800000)) {
                --t;
                fmant <<= 1;
            }
            if (t > 254)
                *fconv = (0x80000000 & *fconv) | 0x7f7fffff;
            else if (t <= 0)
                *fconv = 0;
            else
                *fconv = (0x80000000 & *fconv) | (t << 23) | (0x007fffff & fmant);
        }
    }
}

void
ValueConverter::FPGToFloat(const uint8_t* bytes, float* out, size_t count) const
{
    this->SwapBytes4(bytes, (uint32_t*)out, count);

    for (size_t idx = 0; idx < count; ++idx) {
        const auto g = (int16_t*)(out + idx);
        const int16_t* i = g + 1;

        out[idx] = static_cast<float>((*i) * pow(2, 0xFF & *g));
    }
}

} // namespace segysdk::segyio
