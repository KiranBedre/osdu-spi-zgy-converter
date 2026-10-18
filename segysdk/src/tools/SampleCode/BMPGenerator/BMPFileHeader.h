/*
Copyright 2022 Schlumberger

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

#include <cstddef>
#include <vector>

namespace SampleCode::BMPGenerator
{
class BMPFileHeader
{
private:
    static constexpr size_t FileHeaderSize = 14;
    static constexpr size_t InformationHeaderSize = 40;
    static constexpr size_t bytes_per_pixel = 3;
    static constexpr size_t min_byte_word = 4;

public:
    static std::vector<unsigned char> GetFileHeader(size_t width, size_t height);

    static size_t GetFileSize(const size_t fileHeaderSize, const size_t informationHeaderSize, const size_t height, const size_t width);

    static size_t GetPaddingApplied(const size_t width);

    static std::vector<unsigned char> GetBytes(size_t value);
};

} // namespace SampleCode::BMPGenerator
