/*
Copyright 2021-2022 Schlumberger

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

#include "../SegyIndex/IndexBuilderDataManager.h"

#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{

class IndexBuilderDataManagerTests : public testing::Test
{
protected:
    void SetUp() override
    {}

    bool isFirstTrace = true;
    const int firstInlineNo = 101;
    const int firstCrosslineNo = 10101;
    const int firstCMP = 303;

    const double firstX = 50000;
    const double firstY = 650000;

    const double deltaX = 25;
    const double deltaY = 50;

    const size_t firstOffset = 3600;             // simulate standard SEGY EBCDIC and Binary header size
    const size_t segyTraceSize = 240 + 1500 * 4; // Simulate segy trace with 1500 4-byte samples ...

    void doTest(size_t traceCount, size_t crosslineEnd)
    {
        IndexBuilderDataManager traceDataManager;

        int inlineNo = firstInlineNo;
        int crosslineNo{};
        int cmp = firstCMP;

        for (size_t traceNo = 0; traceNo < traceCount; traceNo++) {
            IndexBuilderData ibd{};
            if (!isFirstTrace && traceNo % crosslineEnd == 0) {
                inlineNo++;
            }
            crosslineNo = firstCrosslineNo + static_cast<int>(traceNo % crosslineEnd);

            ibd.Offset = firstOffset + traceNo * segyTraceSize;
            ibd.InlineNo = inlineNo;
            ibd.CrosslineNo = crosslineNo;
            ibd.X = firstX + (inlineNo - firstInlineNo) * deltaX;
            ibd.Y = firstY + (crosslineNo - firstCrosslineNo) * deltaY;
            ibd.CMP = cmp++;
            ibd.TraceNo = traceNo;

            traceDataManager.WriteData(ibd);
            isFirstTrace = false;
            EXPECT_EQ(traceNo + 1, traceDataManager.GetTraceCount());
        }
        traceDataManager.CloseWriter();

        EXPECT_TRUE(traceDataManager.IsReaderReady());

        isFirstTrace = true;
        size_t traceNo = 0;
        inlineNo = firstInlineNo;
        crosslineNo = firstCrosslineNo;
        cmp = firstCMP;

        while (!traceDataManager.AtReaderEnd()) {
            auto ibd = traceDataManager.ReadNext();
            if (!isFirstTrace && traceNo % crosslineEnd == 0) {
                inlineNo++;
            }

            size_t offset = firstOffset + traceNo * segyTraceSize;
            crosslineNo = static_cast<int>(firstCrosslineNo + traceNo % crosslineEnd);
            double X = firstX + (inlineNo - firstInlineNo) * deltaX;
            double Y = firstY + (crosslineNo - firstCrosslineNo) * deltaY;

            EXPECT_EQ(ibd.Offset, offset);
            EXPECT_EQ(ibd.InlineNo, inlineNo);
            EXPECT_EQ(ibd.CrosslineNo, crosslineNo);
            EXPECT_EQ(ibd.X, X);
            EXPECT_EQ(ibd.Y, Y);
            EXPECT_EQ(ibd.CMP, cmp);
            EXPECT_EQ(ibd.TraceNo, traceNo);

            isFirstTrace = false;
            ++traceNo;
            ++cmp;
        }
        traceDataManager.CloseReader();
    }
};

TEST_F(IndexBuilderDataManagerTests, IndexBuilderWriteTests_TinyFile)
{
    doTest(101, 25);
}

TEST_F(IndexBuilderDataManagerTests, IndexBuilderWriteTests_LargerFile)
{
    doTest(20001, 250);
}

TEST_F(IndexBuilderDataManagerTests, IndexBuilderWriteTests_VeryLargeFile)
{
    doTest(5000001, 250);
}

} // namespace segysdk::segyindex::tests