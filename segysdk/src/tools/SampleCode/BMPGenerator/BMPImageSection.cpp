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

#include "BMPImageSection.h"

#include "BMPFileHeader.h"

#include "../tools/SampleCode/BMPGenerator/BMPInformationHeader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>


namespace SampleCode::BMPGenerator
{
BMPImageSection::BMPImageSection(size_t width, size_t height) :
m_width(width), m_height(height), m_colours(std::vector<Colour>(m_width * m_height))
{
}

Colour
BMPImageSection::GetColour(const size_t x, const size_t y) const
{
    if (x >= m_width) {
        std::cout << "GetColour: x not within valid range [0 .. " << std::to_string(m_width - 1) << "]" << std::endl;
        return  {0, 0, 0};
    }

    if (y >= m_height) {
        std::cout << "GetColour: y not within valid range [0 .. " << std::to_string(m_height - 1) << "]" << std::endl;
        return {0, 0, 0};
    }

    return m_colours[y * m_width + x];
}

void
BMPImageSection::SetColour(const Colour& colour, const size_t x, const size_t y)
{
    if (x >= m_width) {
        std::cout << "SetColour: x = " << std::to_string(x) << " is not within valid range [0 .. " << std::to_string(m_width - 1) << "]" << std::endl;
        return;
    }

    if (y >= m_height) {
        std::cout << "SetColour: y = " << std::to_string(y) << " is not within valid range [0 .. " << std::to_string(m_height - 1) << "]" << std::endl;
        return;
    }

    m_colours[y * m_width + x].r = colour.r;
    m_colours[y * m_width + x].g = colour.g;
    m_colours[y * m_width + x].b = colour.b;
}

void
BMPImageSection::Export(const std::filesystem::path& file_name) const
{
    // Write local file!!
    try {
        std::ofstream f;

        f.open(file_name.c_str(), std::ios::out | std::ios::binary);

        if (!f.is_open()) {
            std::cout << "Could not open output file!." << std::endl;
            return;
        }

        // Each row consists of an integer number of groups of 4 byte blocks
        // Each cell required 3 bytes for their colour value components (r, g and b)

        auto file_header = BMPFileHeader::GetFileHeader(m_width, m_height);
        f.write(reinterpret_cast<char*>(file_header.data()), static_cast<std::streamsize>(file_header.size()));

        auto information_header = BMPInformationHeader::GetInformationHeader(m_width, m_height);
        f.write(reinterpret_cast<char*>(information_header.data()), static_cast<std::streamsize>(information_header.size()));

        const int padding_applied = static_cast<int>(BMPFileHeader::GetPaddingApplied(m_width));
        constexpr char padding[3] = {0, 0, 0};

        for (size_t y = 0; y < m_height; y++) {
            for (size_t x = 0; x < m_width; x++) {
                const auto colour = GetColour(x, y);
                // Colours need to be written 'backwards' because of the byte layout in the exported file
                unsigned char colour_array[] = {colour.b, colour.g, colour.r};
                f.write(reinterpret_cast<char*>(colour_array), 3);
            }

            if (padding_applied > 0) {
                f.write(padding, padding_applied);
            }
        }

        f.close();

        std::cout << "Screen dump created : " << file_name << std::endl;
    } catch (const std::exception& ex) {
        std::cout << "Unable to generate output BMP file: " << file_name.generic_string() << std::endl
                  << "Error: " << ex.what() << std::endl;
    }
}


}
