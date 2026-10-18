/*
Copyright 2021-2023 Schlumberger

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

#include <gtest/gtest.h>

#include "../FileIO/CacheSizeOption.h"

#include "../FileIO/CachedFileReader.h"
#include "../FileIO/DiskFileReader.h"
#include "../Utilities/Environment.h"

#include <filesystem>
#include <numeric>
#include <utility>

namespace segysdk::tests
{

static auto
makeReadCacheParams(const std::size_t offset, const std::size_t size)
{
    return fileio::ReadCacheParams{fileio::FileReadParams{}.withOffset(offset).withSize(size)}.withSegyFileNumber(0);
}

class ReadCacheTest : public ::testing::Test
{
public:
    fileio::ReadCacheParams params = makeReadCacheParams(0, 100);
};

class ReadCacheTestMemory : public ReadCacheTest
{
public:
    bool readsBlocked = false;
};

class ReadCacheTestDisk : public ReadCacheTest
{
public:
    ReadCacheTestDisk()
      : ReadCacheTestDisk(std::filesystem::path{utilities::Environment::instance().SegyLocalTestDataPath()} / "SegyIO")
    {
    }
    explicit ReadCacheTestDisk(const std::filesystem::path& filePath)
      : inputFilenameLower{filePath / "alphabet_lower"},
        inputFilenameUpper{filePath / "alphabet_upper"},
        cacheSize(2, 5, 1, 1024U * 1024U)
    {
    }
    const std::filesystem::path inputFilenameLower;
    const std::filesystem::path inputFilenameUpper;
    const CacheSizeOption cacheSize;
    ByteBuffer readBuf;
    const std::string sampleLowercase{"abcdefghijklmnopqrstuvwxyz"};
    fileio::FileReaders fileReaders = fileio::FileReaders(3, std::make_shared<fileio::DiskFileReader>(inputFilenameLower.string()));
};

static auto
makeFileReadParams(const std::size_t offset, const std::size_t size)
{
    return fileio::FileReadParams{}.withOffset(offset).withSize(size);
}

TEST_F(ReadCacheTestDisk, testReadCache)
{
    auto cfr = fileio::CachedFileReader{std::make_shared<fileio::DiskFileReader>(inputFilenameLower.string()),
                                        std::make_shared<fileio::ReadCache>(cacheSize)};

    // easy case - inside a page
    readBuf = cfr.read(makeFileReadParams(0, 1));
    EXPECT_EQ(readBuf.size(), 1);
    EXPECT_EQ(stringFromBuffer(readBuf), "a");

    // spanning over two pages - start at page start
    readBuf = cfr.read(makeFileReadParams(0, 3));
    EXPECT_EQ(readBuf.size(), 3);
    EXPECT_EQ(stringFromBuffer(readBuf), "abc");

    // spanning over three pages - partial pages at both ends
    readBuf = cfr.read(makeFileReadParams(3, 4));
    EXPECT_EQ(readBuf.size(), 4);
    EXPECT_EQ(stringFromBuffer(readBuf), "defg");
}

TEST_F(ReadCacheTestDisk, testReadCacheSharedSameFile)
{
    const auto sut = std::make_shared<fileio::ReadCache>(cacheSize);
    auto cfr1 = fileio::CachedFileReader{std::make_shared<fileio::DiskFileReader>(inputFilenameLower.string()), sut};
    auto cfr2 = fileio::CachedFileReader{std::make_shared<fileio::DiskFileReader>(inputFilenameLower.string()), sut};

    // read from source 1
    readBuf = cfr1.read(makeFileReadParams(0, 1));
    EXPECT_EQ(readBuf.size(), 1);
    EXPECT_EQ(stringFromBuffer(readBuf), "a");

    // read from source 2
    readBuf = cfr2.read(makeFileReadParams(0, 3));
    EXPECT_EQ(readBuf.size(), 3);
    EXPECT_EQ(stringFromBuffer(readBuf), "abc");
}

TEST_F(ReadCacheTestDisk, testReadCacheSharedDifferentFiles)
{
    auto sut = std::make_shared<fileio::ReadCache>(cacheSize);
    auto cfr1 = fileio::CachedFileReader{std::make_shared<fileio::DiskFileReader>(inputFilenameLower.string()), sut, 0};
    auto cfr2 = fileio::CachedFileReader{std::make_shared<fileio::DiskFileReader>(inputFilenameUpper.string()), sut, 1};

    // read from source 1
    readBuf = cfr1.read(makeFileReadParams(0, 1));
    EXPECT_EQ(readBuf.size(), 1);
    EXPECT_EQ(stringFromBuffer(readBuf), "a");

    // read from source 2
    readBuf = cfr2.read(makeFileReadParams(0, 3));
    EXPECT_EQ(readBuf.size(), 3);
    EXPECT_EQ(stringFromBuffer(readBuf), "ABC");

    // read from source 1
    readBuf = cfr1.read(makeFileReadParams(0, 3));
    EXPECT_EQ(readBuf.size(), 3);
    EXPECT_EQ(stringFromBuffer(readBuf), "abc");
}

TEST_F(ReadCacheTestDisk, readOverTheEdge)
{
    auto reader = fileio::CachedFileReader{std::make_shared<fileio::DiskFileReader>(inputFilenameLower.string()),
                                           std::make_shared<fileio::ReadCache>(CacheSizeOption(2097152, 32, 1, 1048576))};
    const auto buffer = reader.read(makeFileReadParams(0, 100));
    EXPECT_EQ(stringFromBuffer(buffer), sampleLowercase);
}

TEST_F(ReadCacheTestDisk, consolidateMemoryBlocksSimple)
{
    auto cache = fileio::ReadCache{CacheSizeOption(13, 1, 2, 0)};
    EXPECT_EQ(stringFromBuffer(cache.read(params, fileReaders)), sampleLowercase);
}

TEST_F(ReadCacheTestDisk, consolidateMemoryBlocksTail)
{
    auto cache = fileio::ReadCache{CacheSizeOption(20, 1, 2, 0)};
    EXPECT_EQ(stringFromBuffer(cache.read(params, fileReaders)), sampleLowercase);
}

TEST_F(ReadCacheTestDisk, consolidateMemoryBlocksEmptyTail)
{
    auto cache = fileio::ReadCache{CacheSizeOption(13, 1, 3, 0)};
    EXPECT_EQ(stringFromBuffer(cache.read(params, fileReaders)), sampleLowercase);
}

TEST_F(ReadCacheTestDisk, consolidateMemoryBlocksFirstReadsAll)
{
    auto cache = fileio::ReadCache{CacheSizeOption(30, 1, 2, 0)};
    EXPECT_EQ(stringFromBuffer(cache.read(params, fileReaders)), sampleLowercase);
}

class TestFileReader : public fileio::FileReader
{
public:
    TestFileReader(ByteBuffer buffer, bool& readsBlocked);
    ByteBuffer doRead(const fileio::FileReadParams& params) override;

private:
    std::uint64_t getFileSize() override;
    void close() override;

    const ByteBuffer buffer_;
    bool& readsBlocked_;
};

TestFileReader::TestFileReader(ByteBuffer buffer, bool& readsBlocked)
  : fileio::FileReader(""),
    buffer_(std::move(buffer)),
    readsBlocked_(readsBlocked)
{
}

std::uint64_t
TestFileReader::getFileSize()
{
    return buffer_.size();
}

ByteBuffer
TestFileReader::doRead(const fileio::FileReadParams& params)
{
    if (readsBlocked_) {
        return {};
    }
    const auto start = std::min(params.offset_, buffer_.size());
    const auto count = std::min((buffer_.size() - start), params.size_);
    return {std::next(std::cbegin(buffer_), start), std::next(std::cbegin(buffer_), start + count)};
}

void
TestFileReader::close()
{
}

static ByteBuffer
generateBuffer(const std::uint8_t start, const std::size_t count)
{
    ByteBuffer result(count);
    std::iota(result.begin(), result.end(), start);
    return result;
}

TEST_F(ReadCacheTestMemory, cacheOverflow)
{
    const auto sample = generateBuffer(1, 5 * 4);
    auto readers = fileio::FileReaders(1, std::make_shared<TestFileReader>(sample, readsBlocked));
    auto cache = fileio::ReadCache{CacheSizeOption(5, 2, 1, 0)};
    EXPECT_EQ(cache.read(params, readers), sample);
}

TEST_F(ReadCacheTestMemory, cacheOverflowMostRelevantSurvives)
{
    const auto sample = generateBuffer(1, 5 * 3);
    auto readers = fileio::FileReaders(1, std::make_shared<TestFileReader>(sample, readsBlocked));
    auto cache = fileio::ReadCache{CacheSizeOption(5, 2, 1, 0)};
    auto bytesRead = cache.read(makeReadCacheParams(5, 5), readers);
    EXPECT_EQ(bytesRead, generateBuffer(6, 5));
    bytesRead = cache.read(makeReadCacheParams(0, 5), readers);
    EXPECT_EQ(bytesRead, generateBuffer(1, 5));
    bytesRead = cache.read(makeReadCacheParams(0, 5), readers);
    EXPECT_EQ(bytesRead, generateBuffer(1, 5));
    bytesRead = cache.read(makeReadCacheParams(10, 5), readers);
    EXPECT_EQ(bytesRead, generateBuffer(11, 5));
    readsBlocked = true;
    bytesRead = cache.read(makeReadCacheParams(0, 5), readers);
    EXPECT_EQ(bytesRead, generateBuffer(1, 5));
}

} // namespace segysdk::tests
