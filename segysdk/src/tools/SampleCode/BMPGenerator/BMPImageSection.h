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

#include "Colour.h"

#include <filesystem>
#include <vector>

namespace SampleCode::BMPGenerator
{
class BMPImageSection
{
private:
    size_t m_width;
    size_t m_height;
    std::vector<Colour> m_colours;

public:
    BMPImageSection(size_t width, size_t height);

    Colour GetColour(size_t x, size_t y) const;
    void SetColour(const Colour& colour, size_t x, size_t y);

    size_t Width() const { return m_width; }
    size_t Height() const { return m_height; }

    void Export(const std::filesystem::path& file_name) const;
};
}
