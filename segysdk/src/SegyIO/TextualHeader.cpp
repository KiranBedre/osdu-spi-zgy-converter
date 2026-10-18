/*
Copyright 2021 Schlumberger

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

#include "TextualHeader.h"

#include "../FileIO/FileReader.h"
#include "../SegyIO/EbcdicToAscii.h"
#include "../Utilities/StringUtils.h"
#include <utility>

namespace segysdk::segyio
{

TextualHeader::TextualHeader(std::shared_ptr<fileio::FileReader> file, int extendedHeaderCount)
  : file_(std::move(file)),
    extendedHeaderCount_(extendedHeaderCount)
{
}

bool
TextualHeader::isEbcdicEncoded(const char* buffer, bool isExtended)
{
    if (isExtended) {
        if (extendedEncoding_ == TextEncoding::UNKNOWN) {
            extendedEncoding_ = getTextEncoding(buffer, isExtended);
        }
    }
    else {
        if (textualEncoding_ == TextEncoding::UNKNOWN) {
            textualEncoding_ = getTextEncoding(buffer, isExtended);
        }
    }

    auto myEncoding = isExtended ? extendedEncoding_ : textualEncoding_;

    return myEncoding == TextEncoding::EBCDIC;
}

bool
TextualHeader::isLastStanza(const char* stanza, size_t size) const
{
    const char desired[] = "((SEG:ENDTEXT))";
    int z = 0;
    for (size_t i = 0; i < size && z < 15; ++i) {
        if (isspace(stanza[i]))
            continue;
        if (desired[z] != toupper(stanza[i]))
            return false;
        z++;
    }
    // If Z is 0, this stanza was empty, but is still valid
    return z > 0;
}

TextualHeader::TextEncoding
TextualHeader::getTextEncoding(const char* buffer, bool isExtended) const
{
    const unsigned char firstByte = *reinterpret_cast<const unsigned char*>(buffer);
    const unsigned char secondByte = *reinterpret_cast<const unsigned char*>(buffer + 1);
    if ('(' == firstByte && '(' == secondByte) {
        return TextEncoding::ASCII;
    }
    // special test for extended header
    if (isExtended && firstByte == 77 && secondByte == 77) {
        return TextEncoding::EBCDIC;
    }
    if (firstByte == 'C' || firstByte == 'c' || firstByte == ' ') {
        return TextEncoding::ASCII;
    }
    if (firstByte >= 65 && firstByte < 127) {
        return TextEncoding::ASCII;
    }
    return TextEncoding::EBCDIC;
}

void
TextualHeader::convertToAscii(char* buffer, size_t count) const
{
    EbcdicToAscii::bytesToAscii(buffer, buffer, count);
}

std::string
TextualHeader::getHeaderAt(uint64_t offset)
{
    const int size = 3200;

    if (offset + size > file_->getFileSize())
        return "";

    auto buffer = file_->read(fileio::FileReadParams{}.withOffset(offset).withSize(size));
    if (buffer.size() < size)
        return "";

    if (isEbcdicEncoded(reinterpret_cast<const char*>(buffer.data()), offset > 0)) {
        convertToAscii(reinterpret_cast<char*>(buffer.data()), size);
    }

    // make a 0x00 into a space (0x20) to prevent string from being prematurely terminated
    for (size_t i = 0; i < size; ++i) {
        if (buffer[i] == 0x00)
            buffer[i] = 0x20;
    }
    return stringFromBuffer(buffer);
}

int
TextualHeader::getExtendedHeaderCount()
{
    if (extendedHeaderCount_ < 0) {
        int headerCount = 0;

        const uint64_t fileSize = file_->getFileSize();

        for (uint64_t offset = 3600; offset < fileSize - 3200; offset += 3200) {
            std::string header = getHeaderAt(offset);
            if (header.empty()) // we're probably in trouble
                break;

            headerCount++;
            if (isLastStanza(header.c_str(), 3200))
                break;
        }

        extendedHeaderCount_ = headerCount;
    }

    return extendedHeaderCount_;
}

std::string
TextualHeader::getTextualHeader()
{
    return getHeaderAt(0);
}

size_t
TextualHeader::findToken(const std::string& header, const size_t offset, const int headerNo)
{
    size_t pos;

    if (headerNo >= 10) {
        const std::string flag = "C" + std::to_string(headerNo);
        return header.find(flag, offset);
    }
    else {
        // Special consideration to be taken for headers where Header number is less than 10
        // these might have padding either space or leading 0

        const std::string flag1 = "C0" + std::to_string(headerNo); // Very common
        pos = header.find(flag1, offset);
        if (pos != std::string::npos)
            return pos;

        const std::string flag2 = "C " + std::to_string(headerNo); // Very common
        pos = header.find(flag2, offset);
        if (pos != std::string::npos)
            return pos;

        const std::string flag3 = "C" + std::to_string(headerNo) + " "; // Unlikely ...
        return header.find(flag3, offset);
    }
}

std::vector<std::string>
TextualHeader::getHeaderRecordLineSplit(const std::string& header)
{
    // 3200 byte Textual header or extended header where record identifiers are used for each line.
    // Each line will start with C01, C02, C03 ... until the last line C40  or
    // C 1, C 2, C 3 ... until the last line C40

    std::vector<std::string> result;
    size_t offset = 0; // Last token successfully located
    for (int hdrNo1 = 1; hdrNo1 <= 40; hdrNo1++) {
        size_t pos_start = findToken(header, offset, hdrNo1);
        size_t pos_end = std::string::npos;
        if (pos_start != std::string::npos) {
            for (int hdrNo2 = hdrNo1 + 1; hdrNo2 <= 40 && pos_end == std::string::npos; hdrNo2++) {
                pos_end = findToken(header, pos_start + 1, hdrNo2);
            }
            if (pos_end != std::string::npos) {
                size_t length = pos_end - pos_start;
                length = length > 80 ? 80 : length;
                result.push_back(header.substr(offset, length));
                offset += length;
                continue;
            }
        }

        size_t remaindingCharacters = header.size() - offset;
        if (remaindingCharacters <= 80) {
            result.push_back(header.substr(offset, remaindingCharacters));
            break;
        }
    }
    return result;
}

std::vector<std::string>
TextualHeader::getFixedLineLengthSplit(const std::string& header)
{
    // 3200 byte Textual header or extended header without any obvious line break flags.

    std::vector<std::string> result;

    const int bufferLength = header.size();

    int pos = 0;
    while (pos + FixedLineLength <= bufferLength) {
        result.push_back(header.substr(pos, FixedLineLength));
        pos += FixedLineLength;
    }

    if (pos < bufferLength) {
        result.push_back(header.substr(pos));
    }

    return result;
}

std::vector<std::string>
TextualHeader::getTextualHeaderByLine()
{

    const auto result = getHeaderAt(0);

    if (segysdk::utilities::StringUtils::keyMatches(result, "C01") || segysdk::utilities::StringUtils::keyMatches(result, "C40")) {
        return getHeaderRecordLineSplit(result);
    }

    return getFixedLineLengthSplit(result);
}

std::string
TextualHeader::getExtendedHeader(int headerIndex)
{
    if (headerIndex < 0 || headerIndex >= getExtendedHeaderCount())
        return "";

    return getHeaderAt(3600ULL + (headerIndex * 3200ULL));
}

std::vector<std::string>
TextualHeader::getExtendedHeaderByLine(int headerIndex)
{
    std::vector<std::string> header;
    if (headerIndex < 0 || headerIndex >= getExtendedHeaderCount())
        return header;

    return getFixedLineLengthSplit(getHeaderAt(3600ULL + (headerIndex * 3200ULL)));
}

} // namespace segysdk::segyio
