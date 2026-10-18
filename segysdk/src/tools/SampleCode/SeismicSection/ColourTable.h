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

#include "../tools/SampleCode/BMPGenerator/Colour.h"

#include <cstddef>
#include <map>
#include <vector>

namespace SampleCode::ColourTable
{

class ColourTable
{
private:
    static const size_t Entries = 256;
    static const size_t max_entry = Entries - 1;

    std::vector<BMPGenerator::Colour> colour_table_;

private:
    void buildColourTableFromSeeds(const std::map<unsigned char, BMPGenerator::Colour>& seedColours);

public:
    ColourTable();
    explicit ColourTable(const std::map<unsigned char, BMPGenerator::Colour>& seedColours);

public:
    BMPGenerator::Colour GetSampleColour(const float& value, const float& minAmplitude, const float& maxAmplitude) const;
    static size_t MaxColourEntry() { return max_entry; }
    BMPGenerator::Colour GetColourAt(size_t ix) { return colour_table_[ix]; }
};

} // namespace SampleCode::ColourTable
