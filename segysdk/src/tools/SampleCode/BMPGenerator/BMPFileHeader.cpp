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

#include "BMPFileHeader.h"
#include "BMPInformationHeader.h"
namespace SampleCode::BMPGenerator
{

std::vector<unsigned char>
BMPFileHeader::GetBytes(size_t value)
{
    const unsigned char b0 = value >> 0;
    const unsigned char b1 = value >> 8;
    const unsigned char b2 = value >> 16;
    const unsigned char b3 = value >> 24;

    const auto v = {b0, b1, b2, b3};
    return v;
}

size_t
BMPFileHeader::GetFileSize(const size_t fileHeaderSize, const size_t informationHeaderSize, const size_t width, const size_t height)
{
    
    return fileHeaderSize + informationHeaderSize + height * width * bytes_per_pixel + GetPaddingApplied(width) * height;
}

size_t
BMPFileHeader::GetPaddingApplied(const size_t width)
{
    const size_t paddingAmount = (min_byte_word - width * bytes_per_pixel % min_byte_word) % min_byte_word;

    // min_byte_word: 4.  A BMP file wants the exported line to consist of a number of 4-byte words.
    // If the width in number of pixels * bytes per pixel is not a multiple of 4, the remaining bytes needs to be padded out.

    // Bytes per pixel : 3 (one byte each for r, g and b values;

    // (m_width * 3) = number of bytes occupied by colours per row
    // (m_width * 3)  % 4 = number of bytes remainder to be divisible by 4
    // 4 - (m_width * 3)  % 4 = number of padding bytes required
    // (4 - (m_width * 3)  % 4) % 4 = number of padding bytes required but will handle the case where the previous step is 0 if value is divisible by 4!

    return paddingAmount;
}

std::vector<unsigned char>
BMPFileHeader::GetFileHeader(size_t width, size_t height)
{

    std::vector<unsigned char> m_fileHeader(FileHeaderSize);
    m_fileHeader[0] = 'B';
    m_fileHeader[1] = 'M';

    const size_t fileSize = GetFileSize(FileHeaderSize, InformationHeaderSize,  width, height);

    // fileSize will be stored in bytes 2 - 5 with least significant bytes first...
    const std::vector<unsigned char> intBytes = GetBytes(fileSize);
    m_fileHeader[2] = intBytes[0];
    m_fileHeader[3] = intBytes[1];
    m_fileHeader[4] = intBytes[2];
    m_fileHeader[5] = intBytes[3];

    // Reserved 1 (Not used)
    m_fileHeader[6] = 0;
    m_fileHeader[7] = 0;

    // Reserved 2 (Not used)
    m_fileHeader[8] = 0;
    m_fileHeader[9] = 0;

    // Pixel Data offset
    m_fileHeader[10] = FileHeaderSize + InformationHeaderSize;
    m_fileHeader[11] = 0;
    m_fileHeader[12] = 0;
    m_fileHeader[13] = 0;
    return m_fileHeader;
}

} // namespace SampleCode::BMPGenerator
