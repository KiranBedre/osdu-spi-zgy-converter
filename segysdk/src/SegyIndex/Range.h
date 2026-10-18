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

#include "../segysdk_details/DLL_EXPORT.h"

#include <limits>

namespace segysdk::segyindex
{

template<typename T>
struct SEGYSDK_DLL_PUBLIC Range
{

public:
    // Setting Min to max and Max to min is not an error, but has a special meaning that the range has no elements yet
    T Min{std::numeric_limits<T>::max()};
    T Max{std::numeric_limits<T>::min()};

public:
    Range()
    {}

    Range(const T min, const T max)
      : Min(min),
        Max(max)
    {}

    T Delta() const
    {
        return Max - Min;
    }

    void UpdateMinMax(const T value)
    {
        if (Min > Max) {
            Min = value;
            Max = value;
            return;
        }
        if (Min > value)
            Min = value;
        if (Max < value)
            Max = value;
    }
};

} // namespace segysdk::segyindex
