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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <cstdint>
#include <cstdlib>

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC ValueConverter
{
public:
    bool needsSwap{};
    bool isEbcdic{};

public:
    explicit ValueConverter(bool needsSwap = true);

    void Ieee4BytesToFloat(const uint8_t* bytes, float* out, size_t count) const;
    void Ieee8BytesToDouble(const uint8_t* bytes, double* out, size_t count) const;
    void IBM4BytesToFloat(const uint8_t* bytes, float* out, size_t count) const;
    void FPGToFloat(const uint8_t* bytes, float* out, size_t count) const;
    void BytesToInt16(const uint8_t* bytes, int16_t* out, size_t count) const;
    void BytesToInt32(const uint8_t* bytes, int32_t* out, size_t count) const;
    void BytesToUInt16(const uint8_t* bytes, uint16_t* out, size_t count) const;
    void BytesToUInt32(const uint8_t* bytes, uint32_t* out, size_t count) const;

protected:
    // Swapped order of bytes in groups of 2 value is stored to the output buffer
    inline void SwapBytes2(uint8_t const* bytes, uint16_t* out, size_t count) const
    {
        if (this->needsSwap) {
            const auto ints = reinterpret_cast<uint16_t const*>(bytes);
            for (size_t i = 0; i < count; ++i) {
                out[i] =
#ifdef _MSC_VER
                    _byteswap_ushort(ints[i]);
#else
                    __builtin_bswap16(ints[i]);
#endif
            }
        }
    }

    // Swapped order of bytes in groups of 4 value is stored to the output buffer
    inline void SwapBytes4(uint8_t const* bytes, uint32_t* out, size_t count) const
    {
        if (this->needsSwap) {
            const auto ints = reinterpret_cast<uint32_t const*>(bytes);
            for (size_t i = 0; i < count; ++i) {
                out[i] =
#ifdef _MSC_VER
                    _byteswap_ulong(ints[i]);
#else
                    __builtin_bswap32(ints[i]);
#endif
            }
        }
    }

    // Swaped order of bytes in groups of 8 value is stored to the output buffer
    inline void SwapBytes8(uint8_t const* bytes, uint64_t* out, size_t count) const
    {
        if (this->needsSwap) {
            const auto ints = reinterpret_cast<uint64_t const*>(bytes);
            for (size_t i = 0; i < count; ++i) {
                out[i] =
#ifdef _MSC_VER
                    _byteswap_uint64(ints[i]);
#else
                    __builtin_bswap64(ints[i]);
#endif
            }
        }
    }

    // In place convert the bytes from IBM to IEEE representation
    static inline void IBM2Ieee(uint32_t* val);
};

} // namespace segysdk::segyio
