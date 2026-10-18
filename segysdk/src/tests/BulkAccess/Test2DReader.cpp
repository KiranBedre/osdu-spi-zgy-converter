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

#include "../BulkAccess/Segy2DReader.h"
#include "../SegyIndex/SegyIndex.h"
#include "../segysdk/SegySessionFactory.h"
#include "../tests/UtilityLibrary/AutoCleanupTestFile.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstring>
#include <filesystem>

namespace segysdk::bulkaccess::tests
{

class Segy2DReaderTest : public testing::Test
{
public:
    void prepareReader(const AutoCleanupTestFile& tmpFile)
    {
        constexpr bool is2D = true;
        // User sets whether data is 2D or not
        tmpFile.create_index(is2D);
        auto segyFileName = tmpFile.get_path();
        auto jsonConfig = tmpFile.get_segy_config_json_string();

        auto session = segysdk::api::SegySessionFactory({segyFileName}, jsonConfig).CreateSegySession();

        index_ = session->getIndex();

        ASSERT_FALSE(index_->isSegy3D());
        reader_ = session->create2DReader();
    }

    std::vector<float> readByCmp(int cmpStart, int cmpEnd, size_t sampleStart, size_t sampleEnd)
    {
        size_t const totalSize = (std::abs(cmpEnd - cmpStart) + 1) * (sampleEnd - sampleStart + 1);
        std::vector<float> result(totalSize);
        [[maybe_unused]] size_t tracesRead = reader_->readByCmp(cmpStart, cmpEnd, sampleStart, sampleEnd, result.data());

        return result;
    }

    std::vector<float> readByTrace(size_t traceStart, size_t traceEnd, size_t sampleStart, size_t sampleEnd)
    {
        size_t const totalSize = (traceEnd - traceStart + 1) * (sampleEnd - sampleStart + 1);
        std::vector<float> result(totalSize);
        [[maybe_unused]] size_t tracesRead = reader_->readByTraceNo(traceStart, traceEnd, sampleStart, sampleEnd, result.data());

        return result;
    }

    void checkRead(int cmpStart, int cmpEnd, size_t sampleStart, size_t sampleEnd, const std::vector<float>& expected)
    {
        auto checkWithExpected = [&expected] (const std::vector<float>& result) {
            constexpr float const tolerance = 0.001;
            ASSERT_EQ(expected.size(), result.size());
            for (size_t i = 0; i < result.size(); ++i) {
                EXPECT_NEAR(expected[i], result[i], tolerance);
            }
        };
        auto cmpResult = readByCmp(cmpStart, cmpEnd, sampleStart, sampleEnd);
        checkWithExpected(cmpResult);

        size_t const traceStart = index_->getTraceNoForCMP(cmpStart, true);
        size_t const traceEnd = traceStart + cmpEnd - cmpStart;
        auto traceResult = readByTrace(traceStart, traceEnd, sampleStart, sampleEnd);
        checkWithExpected(traceResult);
    }

    std::shared_ptr<segysdk::segyindex::SegyIndex> getIndex() const
    {
        return index_;
    }

private:
    std::unique_ptr<Segy2DReader> reader_;
    std::shared_ptr<segysdk::segyindex::SegyIndex> index_;
};

TEST_F(Segy2DReaderTest, test_read)
{
    std::string fileName = "small2D.sgy";
    AutoCleanupTestFile tmpFile(fileName);
    prepareReader(tmpFile);

    auto index = getIndex();
    auto lineExtents = index->getLineExtents();

    constexpr size_t const readSize = 3;

    //read outside
    std::vector<float> expected = {{-83.1245, 415.622, 498.747,
                                    444.934, -778.635, -593.246,
                                    1155.081, 962.568, 481.284}};

    int cmpStart = lineExtents.getCMPExtent().first();
    size_t sampleStart = 0;
    // read start
    checkRead(cmpStart, cmpStart + readSize - 1, sampleStart, sampleStart + readSize - 1, expected);

    // Check one trace
    expected = {{444.934, -778.635, -593.246}};
    checkRead(cmpStart + 1, cmpStart + 1, sampleStart, sampleStart + readSize - 1, expected);

    cmpStart = cmpStart + 100;
    expected = {{433.406, -108.352, -325.055,
                 -140.579, 246.013, 246.013,
                 -283.074, -880.676, -188.716}};
    checkRead(cmpStart, cmpStart + readSize - 1, sampleStart, sampleStart + readSize - 1, expected);

    sampleStart = 120;
    expected = {{-794.578, -1227.984, -1733.625,
                 -1054.341, -1335.5, -1651.801,
                 -786.318, -1163.75, -1729.898}};
    checkRead(cmpStart, cmpStart + readSize - 1, sampleStart, sampleStart + readSize - 1, expected);
    
    // check section with last samples that lie on the start of the previous check
    sampleStart = 118;
    expected = {{-577.875, -650.109, -794.578,
                 -246.013, -773.184, -1054.341,
                 -188.716, -566.149, -786.318}};
    checkRead(cmpStart, cmpStart + readSize - 1, sampleStart, sampleStart + readSize - 1, expected);


    cmpStart = lineExtents.getCMPExtent().last() - readSize + 1;
    expected = {{498.3630, -387.615, -1688.897,
                 -123.385, -678.620, -1357.240,
                 -472.159, -1127.937, -1416.479}};
    checkRead(cmpStart, cmpStart + readSize - 1, sampleStart, sampleStart + readSize - 1, expected);

    // read past last cmp/trace:
    cmpStart = lineExtents.getCMPExtent().last() - 1;
    expected = {{-123.385, -678.620, -1357.240,
                 -472.159, -1127.937, -1416.479,
                 0, 0, 0}};
    checkRead(cmpStart, cmpStart + readSize - 1, sampleStart, sampleStart + readSize - 1, expected);
}

} // namespace segysdk::bulkaccess::tests
