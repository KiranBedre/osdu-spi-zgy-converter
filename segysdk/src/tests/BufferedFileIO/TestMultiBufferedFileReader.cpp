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

#include "../FileIO/GenericFileManager.h"
#include "../FileIO/MultiBufferedFileReader.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <string>

namespace segysdk::fileio::tests
{

using segysdk::fileio::MultiBufferedFileReader;

namespace details
{

constexpr size_t const readersCount = 3U;
constexpr size_t const readerBufferSize = 4U * 1024U * readersCount;
constexpr size_t const readerReadSize = 4U * 1024U * readersCount;
constexpr size_t const alignment = 0U;
constexpr size_t const readByChunkSize = 2U * 1024U;

} // namespace details

struct MultiBufferedFileReaderUnitTests : public testing::Test
{
    MultiBufferedFileReaderUnitTests()
      : filePathName_{FileReaderTester::getFileFullPathName(FileReaderTester::getDefaultInputFileName())},
        fileLength_{FileReaderTester::getFileSize(filePathName_)}
    {
        /* EMPTY */
    }

    void ContiguousReadingTest(size_t offsetStartReadingAt, size_t amountToRead)
    {
        FileReaderTester frt(details::readerBufferSize,
                             details::readerReadSize,
                             offsetStartReadingAt,
                             amountToRead,
                             details::alignment,
                             details::readByChunkSize);

        // create file objects for the readers
        FileReference inputFileReference(FileReaderTester::fr(filePathName_));

        std::vector<std::shared_ptr<fileio::FileReader>> readers(details::readersCount, nullptr);
        std::generate(
            readers.begin(), readers.end(), [&inputFileReference]() -> auto {
                return fileio::GenericFileManager::createReader(fileio::FileManagerParams{inputFileReference});
            });

        // create the reader
        auto reader = std::make_unique<MultiBufferedFileReader>(std::move(readers));

        size_t const offsetStopReadingAt = offsetStartReadingAt + amountToRead;
        size_t const readerBuffersChunkSize = details::readerReadSize / details::readersCount;
        for (size_t offset = offsetStartReadingAt; offset < offsetStopReadingAt; offset += readerBuffersChunkSize) {
            MultiBufferedFileReader::ReadOffsetMap::value_type::second_type readOffsetItem(details::readersCount, offset);
            for (size_t i = 1; i < readOffsetItem.size(); ++i) {
                size_t const readerFileOffset = i * details::readByChunkSize;
                if (readerFileOffset < fileLength_) {
                    readOffsetItem[i] = readerFileOffset;
                }
            }

            reader->addReadOffsetMapItem(MultiBufferedFileReader::ReadOffsetMap::value_type{offset, std::move(readOffsetItem)});
        }

        // configure the reader
        frt.setReaderConfiguration(reader.get());

        // perform the test
        frt.testReader(reader.get());
    }

    std::string const filePathName_;
    size_t const fileLength_;

protected:
    size_t roundReadAmountToChunkSize(size_t suggestedSize)
    {
        // the total amount to read is rounded to a number of chuck sizes because the partial reading is not supported
        return (suggestedSize / details::readerReadSize) * details::readerReadSize;
    }
};

// Developer note:
// The BASIC tests can be temporarily disabled in case a test fails for reasons outwith the responsibility of this code.
// To disable the tests, simply add the prefix "DISABLED_" to the test name. e.g.
// To reactivate, just remove the "DISABLED_" prefix again.
// TEST_F(MultiBufferedFileReaderUnitTests, SequentialReadingWholeFile) => TEST_F(MultiBufferedFileReaderUnitTests, DISABLED_SequentialReadingWholeFile)

TEST_F(MultiBufferedFileReaderUnitTests, SequentialReadingWholeFile)
{
    size_t const firstOffset = 0U;
    size_t const amountToRead = roundReadAmountToChunkSize(fileLength_);

    ContiguousReadingTest(firstOffset, amountToRead);
}

TEST_F(MultiBufferedFileReaderUnitTests, SequentialReadingFileFirstHalf)
{
    size_t const firstOffset = 0U;
    size_t const amountToRead = roundReadAmountToChunkSize(fileLength_ / 2);

    ContiguousReadingTest(firstOffset, amountToRead);
}

TEST_F(MultiBufferedFileReaderUnitTests, SequentialReadingFileMiddleQuarter)
{
    size_t const firstOffset = fileLength_ / 4;
    size_t const amountToRead = roundReadAmountToChunkSize(fileLength_ / 4);

    ContiguousReadingTest(firstOffset, amountToRead);
}

TEST_F(MultiBufferedFileReaderUnitTests, SequentialReadingFileSecondHalf)
{
    size_t const firstOffset = fileLength_ / 2;
    size_t const amountToRead = roundReadAmountToChunkSize(fileLength_ - firstOffset);

    ContiguousReadingTest(firstOffset, amountToRead);
}

} // namespace segysdk::fileio::tests
