/*
Copyright 2024 Schlumberger

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

#include "../BulkAccess/AsyncReaderImpl.h"
#include "../BulkAccess/ReaderConnector.h"

#include <algorithm>
#include <gtest/gtest.h>

namespace segysdk::bulkaccess::async::tests
{

class ReaderConnector : public bulkaccess::ReaderConnector
{
public:
    explicit ReaderConnector(const std::size_t sizeI = 10, const std::size_t sizeJ = 10, const std::size_t sizeK = 10);
    void withTailValuesMissing(const std::size_t count);

private:
    std::size_t doGetSampleSize() const override;
    std::size_t doGetSizeI() const override;
    std::size_t doGetSizeJ() const override;
    std::size_t doGetSizeK() const override;
    bool doFindTrace(const std::size_t i, const std::size_t j) override;
    std::size_t doGetTraceOffset() const override;
    ByteBuffer doReadSamples(const fileio::FileReadParams& params) const override;
    Samples doConvertBytesToSamples(const ByteBuffer& bytes) const override;
    std::size_t sizeI_;
    std::size_t sizeJ_;
    std::size_t sizeK_;
    std::size_t indexI_ = {0};
    std::size_t indexJ_ = {0};
    std::size_t tailValuesMissing_ = {0};
};

struct TestCase
{
    ExtentSpan spanInline;
    ExtentSpan spanXline;
    ExtentSpan spanSamples;
    Samples referenceValue;
};

using TestCases = std::vector<TestCase>;

static const auto testCases30x20x10 = TestCases{
    {{0, 1}, {0, 1}, {0, 10}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}},
    {{0, 1}, {0, 1}, {0, 11}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}},
    {{0, 1}, {0, 1}, {0, 5}, {1, 2, 3, 4, 5}},
    {{0, 1}, {0, 1}, {5, 5}, {6, 7, 8, 9, 10}},
    {{0, 1}, {0, 1}, {2, 6}, {3, 4, 5, 6, 7, 8}},
    {{1, 1}, {0, 1}, {0, 10}, {11, 12, 13, 14, 15, 16, 17, 18, 19, 20}},
    {{1, 2}, {0, 1}, {0, 10}, {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, //
                               21, 22, 23, 24, 25, 26, 27, 28, 29, 30}},
    {{29, 1}, {19, 1}, {0, 10}, {5991, 5992, 5993, 5994, 5995, 5996, 5997, 5998, 5999, 6000}},
    {{29, 1}, {19, 1}, {0, 11}, {5991, 5992, 5993, 5994, 5995, 5996, 5997, 5998, 5999, 6000}},
    {{15, 1}, {10, 2}, {0, 5}, {3151, 3152, 3153, 3154, 3155, //
                                3451, 3452, 3453, 3454, 3455}},
    {{15, 2}, {10, 2}, {3, 4}, {3154, 3155, 3156, 3157, //
                                3164, 3165, 3166, 3167, //
                                3454, 3455, 3456, 3457, //
                                3464, 3465, 3466, 3467}},
    {{15, 2}, {10, 2}, {9, 100}, {3160, //
                                  3170, //
                                  3460, //
                                  3470}},
    {{28, 3}, {18, 3}, {9, 100}, {5690, //
                                  5700, //
                                  5990, //
                                  6000}},
    {{0, 20}, {0, 1}, {9, 1}, {10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 200}},
};

static const auto testCases20x30x100 = TestCases{
    {{0, 1}, {0, 1}, {0, 10}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}},
    {{0, 1}, {0, 1}, {0, 11}, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}},
    {{0, 1}, {0, 1}, {0, 5}, {1, 2, 3, 4, 5}},
    {{0, 1}, {0, 1}, {5, 5}, {6, 7, 8, 9, 10}},
    {{0, 1}, {0, 1}, {2, 6}, {3, 4, 5, 6, 7, 8}},
    {{1, 1}, {0, 1}, {0, 10}, {101, 102, 103, 104, 105, 106, 107, 108, 109, 110}},
    {{1, 2}, {0, 1}, {0, 10}, {101, 102, 103, 104, 105, 106, 107, 108, 109, 110, //
                               201, 202, 203, 204, 205, 206, 207, 208, 209, 210}},
    {{29, 1}, {19, 1}, {0, 10}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {{0, 1}, {30, 1}, {0, 10}, {}},
    {{19, 1}, {29, 1}, {0, 10}, {59901, 59902, 59903, 59904, 59905, 59906, 59907, 59908, 59909, 59910}},
    {{19, 1}, {29, 1}, {0, 11}, {59901, 59902, 59903, 59904, 59905, 59906, 59907, 59908, 59909, 59910, 59911}},
    {{15, 1}, {10, 2}, {0, 5}, {21501, 21502, 21503, 21504, 21505, //
                                23501, 23502, 23503, 23504, 23505}},
    {{15, 2}, {10, 2}, {3, 4}, {21504, 21505, 21506, 21507, //
                                21604, 21605, 21606, 21607, //
                                23504, 23505, 23506, 23507, //
                                23604, 23605, 23606, 23607}},
};

static void
runTests(async::ReaderConnector connector, const TestCases& testCases)
{
    auto reader = async::ReaderImpl{std::move(connector)};
    for (const auto& testCase : testCases) {
        EXPECT_EQ(reader.read(ReaderParams{}
                                  .withInline(testCase.spanInline)
                                  .withXline(testCase.spanXline)
                                  .withSamples(testCase.spanSamples)),
                  testCase.referenceValue)
            << "inline, xline, samples: {" << testCase.spanInline.start_ << ", " << testCase.spanInline.count_
            << "}, {" << testCase.spanXline.start_ << ", " << testCase.spanXline.count_
            << "}, {" << testCase.spanSamples.start_ << ", " << testCase.spanSamples.count_ << "}";
    }
}

TEST(AsyncReaderTests, extent30x20x10)
{
    runTests(std::make_shared<tests::ReaderConnector>(30, 20, 10), testCases30x20x10);
}

TEST(AsyncReaderTests, extent20x30x100)
{
    runTests(std::make_shared<tests::ReaderConnector>(20, 30, 100), testCases20x30x100);
}

TEST(AsyncReaderTests, extentEmpty)
{
    runTests(
        std::make_shared<tests::ReaderConnector>(0, 0, 0),
        TestCases{
            {{0, 1}, {0, 1}, {0, 5}, {}},
            {{0, 15}, {0, 10}, {0, 10}, {}},
        });
}

TEST(AsyncReaderTests, tailMissing)
{
    auto connector = std::make_shared<tests::ReaderConnector>();
    connector->withTailValuesMissing(3);
    const auto& valuesRead = async::ReaderImpl{connector}.read(ReaderParams{}
                                                                   .withInline({0, 10})
                                                                   .withXline({0, 10})
                                                                   .withSamples({0, 10}));
    ASSERT_EQ(valuesRead.size(), 1000);
    EXPECT_EQ(valuesRead[0], 1);
    EXPECT_EQ(valuesRead[554], 555);
    EXPECT_EQ(valuesRead[996], 997);
    EXPECT_EQ(valuesRead[997], 0);
    EXPECT_EQ(valuesRead[998], 0);
    EXPECT_EQ(valuesRead[999], 0);
}

ReaderConnector::ReaderConnector(const std::size_t sizeI, const std::size_t sizeJ, const std::size_t sizeK)
  : sizeI_(sizeI),
    sizeJ_(sizeJ),
    sizeK_(sizeK)
{}

std::size_t
ReaderConnector::doGetSizeI() const
{
    return sizeI_;
}

std::size_t
ReaderConnector::doGetSizeJ() const
{
    return sizeJ_;
}

std::size_t
ReaderConnector::doGetSizeK() const
{
    return sizeK_;
}

bool
ReaderConnector::doFindTrace(const std::size_t i, const std::size_t j)
{
    if ((i >= sizeI_) || (j >= sizeJ_)) {
        return false;
    }
    indexI_ = i;
    indexJ_ = j;
    return true;
}

std::size_t
ReaderConnector::doGetTraceOffset() const
{
    const auto numSamplesRowBytes = getSampleSize() * sizeK_;
    return indexJ_ * (sizeI_ * numSamplesRowBytes) + indexI_ * numSamplesRowBytes;
}

void
ReaderConnector::withTailValuesMissing(const std::size_t count)
{
    tailValuesMissing_ = count;
}

ByteBuffer
ReaderConnector::doReadSamples(const fileio::FileReadParams& params) const
{
    const auto numSamples = std::min(params.size_ / getSampleSize(), (sizeI_ * sizeJ_ * sizeK_ - tailValuesMissing_) - params.offset_ / getSampleSize());
    auto samples = Samples{};
    samples.reserve(numSamples);
    auto sampleIndex = params.offset_ / getSampleSize();
    std::generate_n(std::back_inserter(samples), numSamples,
                    [&sampleIndex] {
                        return ++sampleIndex;
                    });
    const auto ptr = reinterpret_cast<const std::uint8_t*>(samples.data());
    auto result = ByteBuffer(samples.size() * sizeof(samples[0]));
    std::copy(ptr, ptr + result.size(), result.data());
    return result;
}

Samples
ReaderConnector::doConvertBytesToSamples(const ByteBuffer& bytes) const
{
    auto result = Samples(bytes.size() / getSampleSize());
    const auto ptr = reinterpret_cast<std::uint8_t*>(result.data());
    std::copy(bytes.data(), bytes.data() + result.size() * sizeof(result[0]), ptr);
    return result;
}

std::size_t
ReaderConnector::doGetSampleSize() const
{
    return 4;
}

} // namespace segysdk::bulkaccess::async::tests
