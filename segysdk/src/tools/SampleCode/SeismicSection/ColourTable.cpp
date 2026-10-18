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

#include "ColourTable.h"

#include "../tools/SampleCode/BMPGenerator/Colour.h"

#include <cmath>
#include <map>
#include <utility>

namespace SampleCode::ColourTable
{
ColourTable::ColourTable() : colour_table_(std::vector<BMPGenerator::Colour>(Entries))
{
    std::map<unsigned char, BMPGenerator::Colour> seedColours;
    seedColours[0] = {0, 0, 0};
    seedColours[255] = {255, 255, 255};

    buildColourTableFromSeeds(seedColours);
}

ColourTable::ColourTable(const std::map<unsigned char, BMPGenerator::Colour>& seedColours)
  : colour_table_(std::vector<BMPGenerator::Colour>(Entries))
{
    buildColourTableFromSeeds(seedColours);
}

BMPGenerator::Colour
ColourTable::GetSampleColour(const float& value, const float& minAmplitude, const float& maxAmplitude) const
{
    const double delta = (maxAmplitude - minAmplitude) / static_cast<double>(Entries);

    if (value <= minAmplitude) {
        return colour_table_[0];
    }

    if (value >= maxAmplitude) {
        return colour_table_[max_entry];
    }

    // Set the position so it is around value = 0 rather than up to value = 0 
    auto pos = static_cast<size_t>((value - minAmplitude) / delta);

    if (pos >= max_entry) {
        pos =  max_entry;
    }
    return colour_table_[pos];
}


void
ColourTable::buildColourTableFromSeeds(const std::map<unsigned char, BMPGenerator::Colour>& seedColours)
{
    // Clear previous entries
    colour_table_.clear();
    // Preallocate buffer space
    colour_table_.resize(Entries);

    auto recalculate_colors = [this](int new_colour_shades, int prev_ix,
                 const SampleCode::BMPGenerator::Colour& prev_entry,
                 const std::pair<const unsigned char, SampleCode::BMPGenerator::Colour> &col) {
        const double d_r = (col.second.r - prev_entry.r) * 1.0 / new_colour_shades;
        const double d_g = (col.second.g - prev_entry.g) * 1.0 / new_colour_shades;
        const double d_b = (col.second.b - prev_entry.b) * 1.0 / new_colour_shades;

        int step = 1;
        int ix = prev_ix + 1;
        while (ix < col.first) {
            unsigned char r = prev_entry.r + static_cast<unsigned char>(lround(step * d_r));
            unsigned char g = prev_entry.g + static_cast<unsigned char>(lround(step * d_g));
            unsigned char b = prev_entry.b + static_cast<unsigned char>(lround(step * d_b));

            colour_table_[ix] = {r, g, b};

            step++;
            ix++;
        }
    };

    SampleCode::BMPGenerator::Colour prev_entry;
    int prev_ix = -1;
    for (const auto& col : seedColours) {

        if (col.first == 0) {
            colour_table_[0] = col.second;
        }
        else {
            const int new_colour_shades = col.first - prev_ix + 1;

            if (new_colour_shades > 1) {
                recalculate_colors(new_colour_shades, prev_ix, prev_entry, col);
            }
            colour_table_[col.first] = col.second;
        }

        prev_ix = col.first;
        prev_entry = col.second;
    }
}


} // namespace SampleCode::ColourTable
