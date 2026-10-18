/*
Copyright 2025 Schlumberger

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

#include "Utilities/Containers.h"

#include <string_view>

namespace tool::thumbnail
{

struct BitmapSamples
{
    BitmapSamples(segysdk::Samples&& data, const std::size_t height, std::string_view sliceType);
    BitmapSamples(segysdk::Samples&& data, const std::int64_t height, std::string_view sliceType);
    segysdk::Samples data_;
    std::size_t height_;
    std::string sliceType_;
};

using BitmapSamplesCollection = std::vector<BitmapSamples>;

using ByteBuffer = segysdk::utilities::ByteBuffer;

struct Bitmap
{
    Bitmap(ByteBuffer&& data, const std::size_t width, const std::size_t height);
    const ByteBuffer::value_type* getRowPointer(const std::size_t row) const;
    ByteBuffer data_;
    std::size_t width_;
    std::size_t height_;
};

} // namespace tool::thumbnail
