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

#include "tools/SaveThumbnails/BitmapCreator.h"

#include <cmath>
#include <limits>
#include <map>
#include <tuple>

namespace tool::thumbnail
{

static std::tuple<float, float>
calcAmplitude(const segysdk::Samples& samples)
{
    auto min = std::numeric_limits<float>::max();
    auto max = -min;
    for (const auto sample : samples) {
        max = std::max(sample, max);
        min = std::min(sample, min);
    }
    // make amplitude zero centric if min is negative and max is positive
    if ((min < 0) && (max > 0)) {
        min = -std::min(std::abs(min), max);
        max = std::min(std::abs(min), max);
    }
    return {min, max};
}

struct Colour
{
    std::uint8_t red_{};
    std::uint8_t green_{};
    std::uint8_t blue_{};
};

using ColourTable = std::vector<Colour>;

static void
recalculateColors(ColourTable& table,
                  const double newColourShades,
                  const std::uint8_t prevIndex,
                  const Colour& prevColour,
                  const std::uint8_t index,
                  const Colour& colour)
{
    const double deltaRed = (colour.red_ - prevColour.red_) / newColourShades;
    const double deltaGreen = (colour.green_ - prevColour.green_) / newColourShades;
    const double deltaBlue = (colour.blue_ - prevColour.blue_) / newColourShades;
    for (auto step = 1, i = prevIndex + 1; i < index; ++step, ++i) {
        table[i].red_ = prevColour.red_ + static_cast<std::uint8_t>(std::lround(step * deltaRed));
        table[i].green_ = prevColour.green_ + static_cast<std::uint8_t>(std::lround(step * deltaGreen));
        table[i].blue_ = prevColour.blue_ + static_cast<std::uint8_t>(std::lround(step * deltaBlue));
    }
}

static ColourTable
buildColourTable()
{
    static const auto seedColours = std::map<std::uint8_t, Colour>{
        {0, {0, 255, 255}},     // cyan
        {75, {0, 0, 255}},      // blue
        {76, {0, 0, 0}},        // black
        {128, {250, 250, 250}}, // white
        {181, {165, 42, 42}},   // brown
        {182, {255, 0, 0}},     // red
        {255, {255, 255, 0}},   // yellow
    };
    auto result = ColourTable(256);
    auto prevColour = Colour{};
    std::uint8_t prevIndex = -1;
    for (const auto& [index, colour] : seedColours) {
        if (index == 0) {
            result[0] = colour;
        }
        else {
            if (const double newColourShades = index - prevIndex + 1; newColourShades > 1) {
                recalculateColors(result, newColourShades, prevIndex, prevColour, index, colour);
            }
            result[index] = colour;
        }
        prevIndex = index;
        prevColour = colour;
    }
    return result;
}

static const Colour&
getSampleColour(const float value, const float valueMin, const float valueMax)
{
    static const auto colourTable = buildColourTable();
    if (value <= valueMin) {
        return colourTable.front();
    }
    if (value >= valueMax) {
        return colourTable.back();
    }
    const auto delta = (valueMax - valueMin) / static_cast<double>(colourTable.size());
    const auto position = std::min(
        static_cast<std::size_t>((value - valueMin) / delta),
        colourTable.size() - 1);
    return colourTable[position];
}

Bitmap
createBitmap(const BitmapSamples& samples)
{
    const auto height = std::max<std::size_t>(samples.height_, 1);
    const auto width = samples.data_.size() / height;
    const auto [minValue, maxValue] = calcAmplitude(samples.data_);
    auto colours = ColourTable(width * height);
    for (std::size_t traceIndex = 0; traceIndex < width; ++traceIndex) {
        for (std::size_t sampleIndex = 0; sampleIndex < height; ++sampleIndex) {
            // image builds from bottom up, but traces organized from top down, hence the reverse height indexing
            colours[traceIndex + width * (height - sampleIndex - 1)] = getSampleColour(
                samples.data_[traceIndex * height + sampleIndex],
                minValue,
                maxValue);
        }
    }
    auto result = ByteBuffer{};
    result.reserve(width * height * sizeof(Colour));
    for (std::size_t y = 0; y < height; ++y) {
        for (std::size_t x = 0; x < width; ++x) {
            const auto& colour = colours[y * width + x];
            segysdk::utilities::appendContainer(result, ByteBuffer{colour.red_, colour.green_, colour.blue_});
        }
    }
    return {std::move(result), width, height};
}

} // namespace tool::thumbnail
