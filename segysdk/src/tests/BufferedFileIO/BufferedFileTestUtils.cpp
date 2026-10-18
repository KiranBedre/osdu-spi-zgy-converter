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

#include "BufferedFileTestUtils.h"

#include "../Utilities/Environment.h"
#include "../Utilities/TokenSourceFactory.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>

namespace segysdk::fileio::tests
{
namespace fs = std::filesystem;

FileReaderTester::FileReaderTester(size_t readerBufferSize,
                                   size_t readerReadSize,
                                   size_t offsetStartReadAt,
                                   size_t amountToRead,
                                   size_t alignment,
                                   size_t readByChunkSize)
  : inputFileName_{FileReaderTester::getDefaultInputFileName()},
    readerBufferSize_{readerBufferSize},
    readerReadSize_{readerReadSize},
    offsetStartReadAt_{offsetStartReadAt},
    amountToRead_{amountToRead},
    alignment_{alignment},
    readByChunkSize_{readByChunkSize}
{
    // the input file contents is actually loaded only once per the test fixture
    std::call_once(FileReaderTester::callOnceFlag, FileReaderTester::loadInputFileReference, std::cref(inputFileName_));
}

void
FileReaderTester::setReaderConfiguration(BufferedFileReaderBase* reader)
{
    reader->setBufferSize(readerBufferSize_);
    reader->setReadSize(readerReadSize_);
}

void
FileReaderTester::testReader(BufferedFileReaderBase* reader)
{
    Buffer dataByReader(amountToRead_, '\0');

    invokeReaderUnderTest(*reader, dataByReader);

    compareData(dataByReader);
}

std::string
FileReaderTester::getDefaultInputFileName()
{
    if constexpr (FileReaderTester::useSmallSizeInputFile) {
        return "BulkReader/Testdata8.sgy"; // 2.7 MB
    }
    else {
        return "BulkReader/Testdata32.sgy"; // 26.1 MB
    }
}

std::string
FileReaderTester::getFileFullPathName(std::string const& fileName)
{
    fs::path const testDataPath{utilities::Environment::instance().SegyLocalTestDataPath()};
    if (not fs::exists(testDataPath)) {
        throw std::runtime_error("Please specify the SEGY_LOCALTESTDATA_PATH environment variable to run this test.");
    }

    fs::path const filePathName = testDataPath / fileName;

    if (not fs::exists(filePathName)) {
        throw std::runtime_error("File '" + filePathName.string() + "' not found or not accessible.");
    }

    return filePathName.string();
}

size_t
FileReaderTester::getFileSize(std::string const& filePathName)
{
    return fs::file_size(filePathName);
}

FileReaderTester::Buffer
FileReaderTester::loadFileContents(std::string const& filePathName)
{
    Buffer result;

    size_t const inputFileSize = FileReaderTester::getFileSize(filePathName);

    result.reserve(inputFileSize);

    // open the file:
    std::ifstream file(filePathName, std::ios::binary);

    if (not file.is_open()) {
        throw std::runtime_error("Cannot open '" + filePathName + "' file for reading.");
    }

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // read the data:
    result.insert(result.begin(),
                  std::istream_iterator<char>(file),
                  std::istream_iterator<char>());

    if (inputFileSize not_eq result.size()) {
        throw std::runtime_error("Error reading contents of '" + filePathName + "' file.");
    }

    return result;
}

FileReference
FileReaderTester::fr(const std::string& fileName)
{
    if (fileName.substr(0, FileReaderTester::sdFilePrefix.length()) == FileReaderTester::sdFilePrefix) {
        auto token_source = segysdk::utilities::TokenSourceFactory::getTokenSource();
        const SDCredentials sdCredentials{std::string(),
                                          std::string(),
                                          std::move(token_source)};

        const FileReaderOptions readerOptions{sdCredentials,
                                              CacheSizeOption{FileReaderTester::CacheSizeOption_blockSize,
                                                              FileReaderTester::CacheSizeOption_pages,
                                                              FileReaderTester::CacheSizeOption_blocksPerPage,
                                                              FileReaderTester::CacheSizeOption_chunkSize}};

        return FileReference{fileName, readerOptions};
    }

    return FileReference{fileName, FileReaderOptions::localOnly()};
}

std::unique_ptr<segysdk::utilities::auth::TokenSource>
FileReaderTester::getAuthenticationToken()
{
    auto const fileName = utilities::Environment::instance().sdTokenFile();

    auto tokenSource = segysdk::utilities::TokenSourceFactory::getTokenSource();
    if (!tokenSource->token().empty()) {
        return tokenSource;
    }

    if (fileName.empty()) {
        throw std::runtime_error("The authentication token has not been defined. Please set it in the SD_SVC_TOKEN environment variable or set SD_SVC_TOKEN_FILE to designate a file containing it to run this test.");
    }

    FileReaderTester::Buffer const buffer = FileReaderTester::loadFileContents(fileName);

    return segysdk::utilities::auth::TokenSource::fromToken(
        std::string(buffer.cbegin(), buffer.cend()));
}

void
FileReaderTester::loadInputFileReference(std::string const& inputFileName)
{
    std::string const inputFilePathName = FileReaderTester::getFileFullPathName(inputFileName);
    FileReaderTester::inputFileContents_ = FileReaderTester::loadFileContents(inputFilePathName);
}

void
FileReaderTester::invokeReaderUnderTest(BufferedFileReaderBase& reader, Buffer& outputBuffer) const
{
    for (size_t offset = 0U; offset < amountToRead_; offset += readByChunkSize_) {

        size_t const offsetToReadAt = offsetStartReadAt_ + offset;
        uint8_t const* pointerToInnerBuffer = reader.readAligned(offsetToReadAt, readByChunkSize_, alignment_);

        if (nullptr == pointerToInnerBuffer) {
            throw std::runtime_error("Bad data retrieving of '" + inputFileName_ + "' file using the reader at " + std::to_string(offsetToReadAt) + " offset with chunk size of " + std::to_string(readByChunkSize_));
        }

        std::memcpy(outputBuffer.data() + offset,
                    reinterpret_cast<Buffer::value_type const*>(pointerToInnerBuffer),
                    readByChunkSize_);
    }
}

void
FileReaderTester::compareData(Buffer const& readerBuffer) const
{
    // we do not use std::memcmp(.) to be able to get the index of the difference to occur
    for (size_t offset = 0U; offset < amountToRead_; ++offset) {
        size_t const bufferOffset = offset;
        size_t const fileOffset = offset + offsetStartReadAt_;
        if (*(readerBuffer.data() + bufferOffset) not_eq *(FileReaderTester::inputFileContents_.data() + fileOffset)) {
            throw std::runtime_error("Wrong data has been read from '" + inputFileName_ + "' file at " + std::to_string(fileOffset) + " file offset with chunk size of " + std::to_string(readByChunkSize_) + " where the chunk buffer offset is " + std::to_string(bufferOffset));
        }
    }
}

} // namespace segysdk::fileio::tests
