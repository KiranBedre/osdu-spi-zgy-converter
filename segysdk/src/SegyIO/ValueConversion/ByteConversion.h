/*
Copyright 2021-2022 Schlumberger

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

#include "ByteConverter.h"
#include "ByteValuePosition.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>

namespace segysdk::segyio::valueconversion
{

template<typename T>
class SEGYSDK_DLL_PUBLIC SwapBytes
{
public:
    inline static void
    swapBytes(const uint8_t* bytes, void* result)
    {
        if constexpr (sizeof(T) == 2) {
            const auto* value = (const uint16_t*)bytes;

            *static_cast<uint16_t*>(result) =
#ifdef _MSC_VER
                _byteswap_ushort(*value);
#else
                __builtin_bswap16(*value);
#endif
        }
        else if constexpr (sizeof(T) == 4) {
            const auto* value = (const uint32_t*)bytes;
            *static_cast<uint32_t*>(result) =
#ifdef _MSC_VER
                _byteswap_ulong(*value);
#else
                __builtin_bswap32(*value);
#endif
        }
        else if constexpr (sizeof(T) == 8) {
            const auto* value = (const uint64_t*)bytes;
            *static_cast<uint64_t*>(result) =
#ifdef _MSC_VER
                _byteswap_uint64(*value);
#else
                __builtin_bswap64(*value);
#endif
        }
    }
};

class SEGYSDK_DLL_PUBLIC ByteConversion
{
private:
    struct IbmFloat_t
    {
        uint32_t placeHolder_;
    };

    struct FixedPointFloat_t
    {
        uint32_t placeHolder_;
    };

    inline static float
    ibmFloatToIeee(uint32_t fconv)
    {
        const uint32_t IBM__NAN_MASK = 0x61100000;
        const uint32_t IBM__INF = 0x61100000;
        const uint32_t IBM__MINUS_INF = 0xE1100000;

        if (fconv == 0)
            return 0;

        if (fconv == IBM__INF)
            return std::numeric_limits<float>::infinity();

        if (fconv == IBM__MINUS_INF)
            return -std::numeric_limits<float>::infinity();

        if ((fconv & IBM__NAN_MASK) == IBM__NAN_MASK)
            return std::numeric_limits<float>::quiet_NaN();

        unsigned fmant = 0x00ffffff & fconv;
        if (fmant == 0)
            return 0;

        int t = static_cast<int>((0x7f000000 & fconv) >> 22) - 130;
        while (!(fmant & 0x00800000)) {
            --t;
            fmant <<= 1;
        }

        if (t > 254)
            fconv = (0x80000000 & fconv) | 0x7f7fffff;
        else if (t <= 0)
            fconv = 0;
        else
            fconv = (0x80000000 & fconv) | (t << 23) | (0x007fffff & fmant);
        union {
            float f;
            std::uint32_t i;
        } converter{};
        converter.i = fconv;
        return converter.f;
    }

    template<typename RETURN_TYPE, typename VALUE_TYPE>
    class AlignedConversion
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            return static_cast<RETURN_TYPE>(*(const VALUE_TYPE*)bytes);
        }
    };

    template<typename RETURN_TYPE>
    class AlignedConversion<RETURN_TYPE, IbmFloat_t>
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            return ibmFloatToIeee(*(const uint32_t*)bytes);
        }
    };

    template<typename RETURN_TYPE>
    class AlignedConversion<RETURN_TYPE, FixedPointFloat_t>
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            const auto value = reinterpret_cast<const int16_t*>(bytes);
            return static_cast<RETURN_TYPE>(value[1] * pow(static_cast<RETURN_TYPE>(2), value[0] & 0xFF));
        }
    };

    template<typename RETURN_TYPE, typename VALUE_TYPE>
    class AlignedSwapConversion
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            VALUE_TYPE value;
            SwapBytes<VALUE_TYPE>::swapBytes(bytes, &value);

            return static_cast<RETURN_TYPE>(value);
        }
    };

    template<typename RETURN_TYPE>
    class AlignedSwapConversion<RETURN_TYPE, IbmFloat_t>
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            uint32_t value;
            SwapBytes<IbmFloat_t>::swapBytes(bytes, &value);

            return ibmFloatToIeee(value);
        }
    };

    template<typename RETURN_TYPE>
    class AlignedSwapConversion<RETURN_TYPE, FixedPointFloat_t>
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            int16_t value[2]{};
            SwapBytes<FixedPointFloat_t>::swapBytes(bytes, value);
            return static_cast<RETURN_TYPE>(value[1] * pow(static_cast<RETURN_TYPE>(2), value[0] & 0xFF));
        }
    };

    template<typename RETURN_TYPE, typename VALUE_TYPE>
    class UnalignedConversion
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            VALUE_TYPE value;
            memmove(&value, bytes, sizeof(VALUE_TYPE));

            return static_cast<RETURN_TYPE>(value);
        }
    };

    template<typename RETURN_TYPE>
    class UnalignedConversion<RETURN_TYPE, IbmFloat_t>
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            uint32_t value;
            memmove(&value, bytes, sizeof(uint32_t));

            return ibmFloatToIeee(value);
        }
    };

    template<typename RETURN_TYPE>
    class UnalignedConversion<RETURN_TYPE, FixedPointFloat_t>
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            int16_t value[2]{};
            memmove(value, bytes, sizeof(int16_t) * 2);

            return static_cast<RETURN_TYPE>(value[1] * pow(2, value[0] & 0xFF));
        }
    };

    template<typename RETURN_TYPE, typename VALUE_TYPE>
    class UnalignedSwapConversion
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            VALUE_TYPE value;
            SwapBytes<VALUE_TYPE>::swapBytes(bytes, &value);

            return static_cast<RETURN_TYPE>(value);
        }
    };

    template<typename RETURN_TYPE>
    class UnalignedSwapConversion<RETURN_TYPE, IbmFloat_t>
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            uint32_t value;
            SwapBytes<IbmFloat_t>::swapBytes(bytes, &value);

            return ibmFloatToIeee(value);
        }
    };

    template<typename RETURN_TYPE>
    class UnalignedSwapConversion<RETURN_TYPE, FixedPointFloat_t>
    {
    public:
        inline static RETURN_TYPE
        convertBytes(const uint8_t* bytes)
        {
            int16_t value[2]{};
            SwapBytes<FixedPointFloat_t>::swapBytes(bytes, value);

            return static_cast<RETURN_TYPE>(value[1] * pow(static_cast<RETURN_TYPE>(2), value[0] & 0xFF));
        }
    };

    template<template<typename, typename> class CONVERSION, typename VALUE_TYPE>
    class Converter : public ByteConverter
    {
    public:
        Converter(){};

        Converter(const Converter& other) = delete;
        Converter& operator=(const Converter& other) = delete;

    private:
        virtual double
        convert(const uint8_t* bytes) const
        {
            return CONVERSION<double, VALUE_TYPE>::convertBytes(bytes);
        }

        virtual void
        convert(const uint8_t* bytes, double* values, const ByteValuePositions& positions) const
        {
            const ByteValuePosition* position = positions.data();
            const size_t count = positions.size();
            for (size_t i = 0; i < count; ++i) {
                values[position[i].getValueOffset()] = CONVERSION<double, VALUE_TYPE>::convertBytes(bytes + position[i].getByteOffset());
            }
        }

        virtual void
        convert(const uint8_t* bytes, float* values, size_t count) const
        {
            for (size_t i = 0; i < count; ++i) {
                values[i] = CONVERSION<float, VALUE_TYPE>::convertBytes(bytes + i * sizeof(VALUE_TYPE));
            }
        }

        virtual void
        convert(const uint8_t* bytes, float* values, size_t count, float scaleFactor) const
        {
            for (size_t i = 0; i < count; ++i) {
                values[i] = CONVERSION<float, VALUE_TYPE>::convertBytes(bytes + i * sizeof(VALUE_TYPE)) * scaleFactor;
            }
        }
    };

    class ErrorConverter : public ByteConverter
    {
    public:
        ErrorConverter(){};

        ErrorConverter(const ErrorConverter& other) = delete;
        ErrorConverter& operator=(const ErrorConverter& other) = delete;

    private:
        virtual double
        convert(const uint8_t* /*bytes*/) const
        {
            return std::numeric_limits<double>::quiet_NaN();
        }

        virtual void
        convert(const uint8_t* /*bytes*/, double* values, const ByteValuePositions& positions) const
        {
            const ByteValuePosition* position = positions.data();
            const size_t count = positions.size();
            for (size_t i = 0; i < count; ++i) {
                values[position[i].getValueOffset()] = std::numeric_limits<double>::quiet_NaN();
            }
        }

        virtual void
        convert(const uint8_t* /*bytes*/, float* values, size_t count) const
        {
            for (size_t i = 0; i < count; ++i) {
                values[i] = std::numeric_limits<float>::quiet_NaN();
            }
        }

        virtual void
        convert(const uint8_t* /*bytes*/, float* values, size_t count, float /*scaleFactor*/) const
        {
            for (size_t i = 0; i < count; ++i) {
                values[i] = std::numeric_limits<float>::quiet_NaN();
            }
        }
    };

    template<template<typename, typename> class CONVERSION>
    static std::unique_ptr<ByteConverter>
    createConverter(SampleFormat valueType)
    {
        switch (valueType) {
        case SampleFormat::FixedPointWithGain4: return std::make_unique<Converter<CONVERSION, FixedPointFloat_t>>();
        case SampleFormat::IBMFloat4: return std::make_unique<Converter<CONVERSION, IbmFloat_t>>();
        case SampleFormat::IeeeFloat4: return std::make_unique<Converter<CONVERSION, float>>();
        case SampleFormat::IeeeFloat8: return std::make_unique<Converter<CONVERSION, double>>();
        case SampleFormat::INT2: return std::make_unique<Converter<CONVERSION, int16_t>>();
        case SampleFormat::INT4: return std::make_unique<Converter<CONVERSION, int32_t>>();
        case SampleFormat::UINT2: return std::make_unique<Converter<CONVERSION, uint16_t>>();
        case SampleFormat::UINT4: return std::make_unique<Converter<CONVERSION, uint32_t>>();
        case SampleFormat::INT1: return std::make_unique<Converter<AlignedConversion, int8_t>>();
        case SampleFormat::UINT1: return std::make_unique<Converter<AlignedConversion, uint8_t>>();
        default: return std::make_unique<ErrorConverter>();
        }
    }

    template<template<typename, typename> class CONVERSION>
    static double
    convertValue(const uint8_t* bytes, SampleFormat valueType)
    {
        switch (valueType) {
        case SampleFormat::FixedPointWithGain4: return CONVERSION<double, FixedPointFloat_t>::convertBytes(bytes);
        case SampleFormat::IBMFloat4: return CONVERSION<double, IbmFloat_t>::convertBytes(bytes);
        case SampleFormat::IeeeFloat4: return CONVERSION<double, float>::convertBytes(bytes);
        case SampleFormat::IeeeFloat8: return CONVERSION<double, double>::convertBytes(bytes);
        case SampleFormat::INT2: return CONVERSION<double, int16_t>::convertBytes(bytes);
        case SampleFormat::INT4: return CONVERSION<double, int32_t>::convertBytes(bytes);
        case SampleFormat::UINT2: return CONVERSION<double, uint16_t>::convertBytes(bytes);
        case SampleFormat::UINT4: return CONVERSION<double, uint32_t>::convertBytes(bytes);

        case segysdk::segyio::SampleFormat::INT1: [[fallthrough]];
        case segysdk::segyio::SampleFormat::INT3: [[fallthrough]];
        case segysdk::segyio::SampleFormat::UINT1: [[fallthrough]];
        case segysdk::segyio::SampleFormat::UINT3: [[fallthrough]];
        default: return std::numeric_limits<double>::quiet_NaN();
        }
    }

public:
    ByteConversion() = delete;

    inline static double
    convertValue(const uint8_t* bytes, SampleFormat valueType, bool aligned, bool swapBytes)
    {
        if (aligned) {
            if (swapBytes)
                return convertValue<AlignedSwapConversion>(bytes, valueType);
            else
                return convertValue<AlignedConversion>(bytes, valueType);
        }
        else {
            if (swapBytes)
                return convertValue<UnalignedSwapConversion>(bytes, valueType);
            else
                return convertValue<UnalignedConversion>(bytes, valueType);
        }
    }

    static std::unique_ptr<ByteConverter> createConverter(SampleFormat valueType, bool aligned, bool swapBytes);
};

} // namespace segysdk::segyio::valueconversion
