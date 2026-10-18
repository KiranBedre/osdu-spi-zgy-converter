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

#include <gtest/gtest.h>

#include "../FileIO/FileReader.h"
#include "../FileIO/PagedFileReader.h"

using namespace segysdk::fileio;

static const char* mockData = "abcdefgh";

static constexpr size_t alignedOffset = 0;
static constexpr size_t unalignedOffset = 1;

static constexpr size_t lessThanOnePage = 1;
static constexpr size_t pageSize = 2;

struct Read
{
    const uint64_t offset;
    const size_t bufferSize;

    bool operator==(const Read& other) const { return offset == other.offset && bufferSize == other.bufferSize; }
};

static constexpr Read readFirstPage{0, pageSize};
static constexpr Read readSecondPage{pageSize, pageSize};

static auto
makeFileReadParams(const std::size_t offset, const std::size_t size)
{
    return segysdk::fileio::FileReadParams{}.withOffset(offset).withSize(size);
}

class MockFileReader final : public ::FileReader
{
public:
    MockFileReader()
      : FileReader("mock") { setBufferSize(2); }
    ~MockFileReader() override = default;

    uint64_t getFileSize() override { return strlen(mockData); }

    segysdk::ByteBuffer doRead(const FileReadParams& params) override
    {
        reads_.push_back({params.offset_, params.size_});
        return {mockData + params.offset_, mockData + params.offset_ + params.size_};
    }
    void close() override {}

    void clearReadSequence()
    {
        reads_.clear();
    }

    void expectReadSequence(std::vector<Read> expected)
    {
        ASSERT_EQ(reads_.size(), expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i) {
            EXPECT_EQ(reads_[i], expected[i]);
        }
    }

private:
    std::vector<Read> reads_{};
};

TEST(PagedFileReaderArgs, Read0Bytes)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto readBytes = sut.read(makeFileReadParams(alignedOffset, 0));

    EXPECT_EQ(readBytes.size(), 0);
    fr->expectReadSequence({});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "");
}

TEST(PagedFileReaderArgs, ReadBeyondEof)
{
    // TODO this should probably be an error

    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto readBytes = sut.read(makeFileReadParams(strlen(mockData), 1));

    EXPECT_EQ(readBytes.size(), 0);
    fr->expectReadSequence({});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "");
}

TEST(SingleRead, ReadLessThanOnePageAligned_ReadWholePage)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto readBytes = sut.read(makeFileReadParams(alignedOffset, lessThanOnePage));

    EXPECT_EQ(readBytes.size(), lessThanOnePage);
    fr->expectReadSequence({readFirstPage});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "a");
}

TEST(SingleRead, ReadLessThanOnePageUnligned_ReadWholePage)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto readBytes = sut.read(makeFileReadParams(unalignedOffset, lessThanOnePage));

    EXPECT_EQ(readBytes.size(), lessThanOnePage);
    fr->expectReadSequence({readFirstPage});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "b");
}

TEST(SingleRead, ReadOnePageAligned_ReadWholePage)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto readBytes = sut.read(makeFileReadParams(alignedOffset, pageSize));

    EXPECT_EQ(readBytes.size(), pageSize);
    fr->expectReadSequence({readFirstPage});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "ab");
}

TEST(SingleRead, ReadOnePageUnaligned_ReadPaged)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto readBytes = sut.read(makeFileReadParams(unalignedOffset, pageSize));

    EXPECT_EQ(readBytes.size(), pageSize);
    fr->expectReadSequence({readFirstPage, readSecondPage});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "bc");
}

TEST(SingleRead, ReadTwoPagesAligned_ReadPaged)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto bytesToRead = 2 * pageSize;
    const auto readBytes = sut.read(makeFileReadParams(alignedOffset, bytesToRead));

    EXPECT_EQ(readBytes.size(), bytesToRead);
    fr->expectReadSequence({Read({alignedOffset, bytesToRead})});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "abcd");
}

TEST(SingleRead, ReadTwoPagesUnaligned_Passthrough)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto bytesToRead = 2 * pageSize;
    const auto readBytes = sut.read(makeFileReadParams(unalignedOffset, bytesToRead));

    EXPECT_EQ(readBytes.size(), bytesToRead);
    fr->expectReadSequence({Read({unalignedOffset, bytesToRead})});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "bcde");
}

TEST(SingleRead, ReadMoreThanTwoPages_Passthrough)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto bytesToRead = 2 * pageSize + 1;
    const auto readBytes = sut.read(makeFileReadParams(0, bytesToRead));

    EXPECT_EQ(readBytes.size(), bytesToRead);
    fr->expectReadSequence({Read({0, bytesToRead})});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "abcde");
}

TEST(ReadFromAlreadyCachedPages, ReadSameContent_ServedFromCache)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    const auto bytesToRead = pageSize;
    sut.read(makeFileReadParams(alignedOffset, bytesToRead));

    fr->clearReadSequence();
    const auto readBytes = sut.read(makeFileReadParams(alignedOffset, bytesToRead));

    EXPECT_EQ(readBytes.size(), bytesToRead);
    fr->expectReadSequence({});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "ab");
}

TEST(ReadFromAlreadyCachedPages, ReadContentFromSameBlock_ServedFromCache)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    sut.read(makeFileReadParams(alignedOffset, 1));

    fr->clearReadSequence();
    const auto readBytes = sut.read(makeFileReadParams(alignedOffset + 1, 1));

    EXPECT_EQ(readBytes.size(), 1);
    fr->expectReadSequence({});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "b");
}

TEST(ReadFromAlreadyCachedPages, ReadContentBeyondCachedBlock_OldPageServedFromCache)
{
    const auto fr = std::make_shared<MockFileReader>();
    PagedFileReader sut(fr);
    sut.read(makeFileReadParams(alignedOffset, 1));
    fr->expectReadSequence({readFirstPage});

    fr->clearReadSequence();
    const auto readBytes = sut.read(makeFileReadParams(alignedOffset + 1, pageSize));

    EXPECT_EQ(readBytes.size(), pageSize);
    fr->expectReadSequence({readSecondPage});
    EXPECT_EQ(segysdk::stringFromBuffer(readBytes), "bc");
}
