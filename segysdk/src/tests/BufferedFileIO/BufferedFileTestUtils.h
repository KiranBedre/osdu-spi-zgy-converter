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

#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/FileReference.h"
#include "../Utilities/auth/TokenSource.h"

#include <cstddef>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

namespace segysdk::fileio::tests
{
using segysdk::fileio::BufferedFileReaderBase;

struct FileReaderTester
{
    using Buffer = std::vector<char>;

    static constexpr bool const useSmallSizeInputFile = true;

    FileReaderTester(size_t readerBufferSize,
                     size_t readerReadSize,
                     size_t offsetStartReadAt,
                     size_t amountToRead,
                     size_t alignment,
                     size_t readByChunkSize);

    void setReaderConfiguration(BufferedFileReaderBase* reader);
    void testReader(BufferedFileReaderBase* reader);

    static std::string getDefaultInputFileName();
    static std::string getFileFullPathName(std::string const& fileName);
    static size_t getFileSize(std::string const& filePathName);
    static Buffer loadFileContents(std::string const& filePathName);
    static FileReference fr(const std::string& fileName);
    static std::unique_ptr<segysdk::utilities::auth::TokenSource> getAuthenticationToken();

    FileReaderTester(FileReaderTester const&) = delete;
    FileReaderTester& operator=(FileReaderTester const&) = delete;
    FileReaderTester(FileReaderTester&&) = delete;
    FileReaderTester& operator=(FileReaderTester&&) = delete;

private:
    static void loadInputFileReference(std::string const& inputFileName);
    void invokeReaderUnderTest(BufferedFileReaderBase& reader, Buffer& outputBuffer) const;
    void compareData(Buffer const& readerBuffer) const;

private:
    std::string const inputFileName_;
    size_t readerBufferSize_{};
    size_t readerReadSize_{};
    size_t offsetStartReadAt_{};
    size_t amountToRead_{};
    size_t alignment_{};
    size_t readByChunkSize_{};

    inline static std::once_flag callOnceFlag;
    inline static Buffer inputFileContents_;

    static constexpr std::string_view const sdFilePrefix = "sd://";
    static constexpr size_t const CacheSizeOption_blockSize = 2U * 1024U * 1024U;
    static constexpr size_t const CacheSizeOption_pages = 32U;
    static constexpr size_t const CacheSizeOption_blocksPerPage = 1U;
    static constexpr size_t const CacheSizeOption_chunkSize = 1024U * 1024U;
};

} // namespace segysdk::fileio::tests
