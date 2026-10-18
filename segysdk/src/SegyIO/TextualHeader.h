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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReader.h"

#include <memory>
#include <string>
#include <vector>

namespace segysdk::segyio
{
class SEGYSDK_DLL_PUBLIC TextualHeader
{
private:
    enum class TextEncoding
    {
        UNKNOWN,
        ASCII,
        EBCDIC
    };

    const int FixedLineLength = 80;


private:
    std::shared_ptr<fileio::FileReader> file_;
    TextEncoding textualEncoding_{TextEncoding::UNKNOWN};
    TextEncoding extendedEncoding_{TextEncoding::UNKNOWN};
    int extendedHeaderCount_;

private:
    bool isEbcdicEncoded(const char* buffer, bool isExtended);
    bool isLastStanza(const char* stanza, size_t size) const;

    void convertToAscii(char* buffer, size_t count) const;

    TextEncoding getTextEncoding(const char* buffer, bool isExtended) const;
    std::string getHeaderAt(uint64_t offset);

    std::vector<std::string> getFixedLineLengthSplit(const std::string& header);
    std::vector<std::string> getHeaderRecordLineSplit(const std::string& header);
    size_t findToken(const std::string& header, const size_t offset, const int headerNo);

public:
    TextualHeader(std::shared_ptr<fileio::FileReader> file, int extendedHeaderCount);

    int getExtendedHeaderCount();
    std::string getTextualHeader();
    std::vector<std::string> getTextualHeaderByLine();
    std::string getExtendedHeader(int headerIndex);
    std::vector<std::string> getExtendedHeaderByLine(int headerIndex);
};

} // namespace segysdk::segyio
