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

#pragma warning(push)
#pragma warning(disable : 4995) // 'function': name was marked as #pragma deprecated
#include <cstddef>
#include <stdint.h>
#include <vector>
#pragma warning(pop)

namespace segysdk::segyio::valueconversion
{

struct SEGYSDK_DLL_PUBLIC ByteValuePosition
{
private:
    size_t byteOffset_;
    int64_t valueOffset_;

public:
    ByteValuePosition(size_t byteOffset, int64_t valueOffset)
      : byteOffset_(byteOffset), valueOffset_(valueOffset)
    {}

    inline size_t getByteOffset() const { return byteOffset_; }
    inline int64_t getValueOffset() const { return valueOffset_; }

    inline void setByteOffset(size_t byteOffset) { byteOffset_ = byteOffset; }
    inline void setValueOffset(int64_t valueOffset) { valueOffset_ = valueOffset; }
};

typedef std::vector<ByteValuePosition> ByteValuePositions;

} // namespace segysdk::segyio::valueconversion
