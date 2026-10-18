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

#include "BMPInformationHeader.h"

#include "BMPFileHeader.h"

namespace SampleCode::BMPGenerator
{

std::vector<unsigned char>
BMPInformationHeader::GetInformationHeader(const size_t width, const size_t height)
{
    constexpr int InformationHeaderSize = 40;

    std::vector<unsigned char> m_informationHeader(InformationHeaderSize);

    // Header size
    m_informationHeader[0] = static_cast<unsigned char>(InformationHeaderSize);
    m_informationHeader[1] = 0;
    m_informationHeader[2] = 0;
    m_informationHeader[3] = 0;

    // Image width
    auto intBytes = BMPFileHeader::GetBytes(width);
    m_informationHeader[4] = intBytes[0];
    m_informationHeader[5] = intBytes[1];
    m_informationHeader[6] = intBytes[2];
    m_informationHeader[7] = intBytes[3];

    // Image Height
    intBytes = BMPFileHeader::GetBytes(height);
    m_informationHeader[8] = intBytes[0];
    m_informationHeader[9] = intBytes[1];
    m_informationHeader[10] = intBytes[2];
    m_informationHeader[11] = intBytes[3];

    // Planes

    m_informationHeader[12] = 1;
    m_informationHeader[13] = 0;

    // Bits per pixel
    m_informationHeader[14] = 24;
    m_informationHeader[15] = 0;

    // Compression (No compression)
    m_informationHeader[16] = 0;
    m_informationHeader[17] = 0;
    m_informationHeader[18] = 0;
    m_informationHeader[19] = 0;

    // Image size (No compression)
    m_informationHeader[20] = 0;
    m_informationHeader[21] = 0;
    m_informationHeader[22] = 0;
    m_informationHeader[23] = 0;

    // X Pixels oer meter (not specified)
    m_informationHeader[24] = 0;
    m_informationHeader[25] = 0;
    m_informationHeader[26] = 0;
    m_informationHeader[27] = 0;

    // Y Pixels oer meter (not specified)
    m_informationHeader[28] = 0;
    m_informationHeader[29] = 0;
    m_informationHeader[30] = 0;
    m_informationHeader[31] = 0;

    // Total colours (colour palette not used)
    m_informationHeader[32] = 0;
    m_informationHeader[33] = 0;
    m_informationHeader[34] = 0;
    m_informationHeader[35] = 0;

    // Important colours (Generally Ignored)
    m_informationHeader[36] = 0;
    m_informationHeader[37] = 0;
    m_informationHeader[38] = 0;
    m_informationHeader[39] = 0;

    return m_informationHeader;
}

} // namespace SampleCode::BMPGenerator
