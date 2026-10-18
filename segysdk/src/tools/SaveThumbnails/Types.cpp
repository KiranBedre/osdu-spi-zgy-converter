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

#include "tools/SaveThumbnails/Types.h"

namespace tool::thumbnail
{

BitmapSamples::BitmapSamples(segysdk::Samples&& data, const std::int64_t height, std::string_view sliceType)
  : BitmapSamples(std::move(data), static_cast<std::size_t>(height), sliceType)
{
}

BitmapSamples::BitmapSamples(segysdk::Samples&& data, const std::size_t height, std::string_view sliceType)
  : data_{std::move(data)}, height_{height}, sliceType_{sliceType}
{
}

Bitmap::Bitmap(ByteBuffer&& data, const std::size_t width, const std::size_t height)
  : data_{std::move(data)}, width_{width}, height_{height}
{
}

const ByteBuffer::value_type*
Bitmap::getRowPointer(const std::size_t row) const
{
    return &data_[(height_ - row - 1) * width_ * 3];
}

} // namespace tool::thumbnail
