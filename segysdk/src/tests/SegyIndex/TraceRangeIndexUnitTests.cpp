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

#include "TraceRangeTestUtils.h"

#include "../SegyIndex/Utilities/TraceIndexMap.h"

#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{
using trace_range_map::TraceIndexMapKey;

class TraceRangeIndexTest : public testing::Test
{
    /* EMPTY */
};

// Developer note:
// The BASIC tests can be temporarily disabled in case a test fails for reasons outwith the responsibility of this code.
// To disable the tests, simply add the prefix "DISABLED_" to the test name. e.g.
// To reactivate, just remove the "DISABLED_" prefix again.
// TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveIncIncIndexesOne) => TEST_F(TraceRangeIndexTest, DISABLED_FillAndReadIndexMap_PositiveIncIncIndexesOne)

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveIncIncIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisPositiveIncIncIndexes_One(),
                                /*pri*/ TraceIndexMapKey::Order::Asc,
                                /*seci*/ TraceIndexMapKey::Order::Asc,
                                trt.getTA_One,
                                trt.getPriPInc,
                                trt.getSecPInc,
                                trt.getTO_One,
                                trt.getStepP(), trt.getStepP(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveIncIncIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisPositiveIncIncIndexes_Two(),
                                /*pri*/ TraceIndexMapKey::Order::Asc,
                                /*seci*/ TraceIndexMapKey::Order::Asc,
                                trt.getTA_Two,
                                trt.getPriPInc,
                                trt.getSecPInc,
                                trt.getTO_Two,
                                trt.getStepP(), trt.getStepP(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveIncDecIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisPositiveIncDecIndexes_One(),
                                /*pri*/ TraceIndexMapKey::Order::Asc,
                                /*seci*/ TraceIndexMapKey::Order::Desc,
                                trt.getTA_One,
                                trt.getPriPInc,
                                trt.getSecPDec,
                                trt.getTO_One,
                                trt.getStepP(), trt.getStepN(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveIncDecIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisPositiveIncDecIndexes_Two(),
                                /*pri*/ TraceIndexMapKey::Order::Asc,
                                /*seci*/ TraceIndexMapKey::Order::Desc,
                                trt.getTA_Two,
                                trt.getPriPInc,
                                trt.getSecPDec,
                                trt.getTO_Two,
                                trt.getStepP(), trt.getStepN(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveDecIncIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisPositiveDecIncIndexes_One(),
                                /*pri*/ TraceIndexMapKey::Order::Desc,
                                /*seci*/ TraceIndexMapKey::Order::Asc,
                                trt.getTA_One,
                                trt.getPriPDec,
                                trt.getSecPInc,
                                trt.getTO_One,
                                trt.getStepN(), trt.getStepP(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveDecIncIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisPositiveDecIncIndexes_Two(),
                                /*pri*/ TraceIndexMapKey::Order::Desc,
                                /*seci*/ TraceIndexMapKey::Order::Asc,
                                trt.getTA_Two,
                                trt.getPriPDec,
                                trt.getSecPInc,
                                trt.getTO_Two,
                                trt.getStepN(), trt.getStepP(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveDecDecIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisPositiveDecDecIndexes_One(),
                                /*pri*/ TraceIndexMapKey::Order::Desc,
                                /*seci*/ TraceIndexMapKey::Order::Desc,
                                trt.getTA_One,
                                trt.getPriPDec,
                                trt.getSecPDec,
                                trt.getTO_One,
                                trt.getStepN(), trt.getStepN(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_PositiveDecDecIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisPositiveDecDecIndexes_Two(),
                                /*pri*/ TraceIndexMapKey::Order::Desc,
                                /*seci*/ TraceIndexMapKey::Order::Desc,
                                trt.getTA_Two,
                                trt.getPriPDec,
                                trt.getSecPDec,
                                trt.getTO_Two,
                                trt.getStepN(), trt.getStepN(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_NegativeIncIncIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisNegativeIncIncIndexes_One(),
                                /*pri*/ TraceIndexMapKey::Order::Asc,
                                /*seci*/ TraceIndexMapKey::Order::Asc,
                                trt.getTA_One,
                                trt.getPriNInc,
                                trt.getSecNInc,
                                trt.getTO_One,
                                trt.getStepP(), trt.getStepP(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_NegativeIncIncIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisNegativeIncIncIndexes_Two(),
                                /*pri*/ TraceIndexMapKey::Order::Asc,
                                /*seci*/ TraceIndexMapKey::Order::Asc,
                                trt.getTA_Two,
                                trt.getPriNInc,
                                trt.getSecNInc,
                                trt.getTO_Two,
                                trt.getStepP(), trt.getStepP(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_NegativeIncDecIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisNegativeIncDecIndexes_One(),
                                /*pri*/ TraceIndexMapKey::Order::Asc,
                                /*seci*/ TraceIndexMapKey::Order::Desc,
                                trt.getTA_One,
                                trt.getPriNInc,
                                trt.getSecNDec,
                                trt.getTO_One,
                                trt.getStepP(), trt.getStepN(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_NegativeIncDecIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisNegativeIncDecIndexes_Two(),
                                /*pri*/ TraceIndexMapKey::Order::Asc,
                                /*seci*/ TraceIndexMapKey::Order::Desc,
                                trt.getTA_Two,
                                trt.getPriNInc,
                                trt.getSecNDec,
                                trt.getTO_Two,
                                trt.getStepP(), trt.getStepN(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_NegativeDecIncIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisNegativeDecIncIndexes_One(),
                                /*pri*/ TraceIndexMapKey::Order::Desc,
                                /*seci*/ TraceIndexMapKey::Order::Asc,
                                trt.getTA_One,
                                trt.getPriNDec,
                                trt.getSecNInc,
                                trt.getTO_One,
                                trt.getStepN(), trt.getStepP(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_NegativeDecIncIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisNegativeDecIncIndexes_Two(),
                                /*pri*/ TraceIndexMapKey::Order::Desc,
                                /*seci*/ TraceIndexMapKey::Order::Asc,
                                trt.getTA_Two,
                                trt.getPriNDec,
                                trt.getSecNInc,
                                trt.getTO_Two,
                                trt.getStepN(), trt.getStepP(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_NegativeDecDecIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisNegativeDecDecIndexes_One(),
                                /*pri*/ TraceIndexMapKey::Order::Desc,
                                /*seci*/ TraceIndexMapKey::Order::Desc,
                                trt.getTA_One,
                                trt.getPriNDec,
                                trt.getSecNDec,
                                trt.getTO_One,
                                trt.getStepN(), trt.getStepN(),
                                trt.defaultSampleFormat_);
}

TEST_F(TraceRangeIndexTest, FillAndReadIndexMap_NegativeDecDecIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillAndReadIndexMap(trt.getTisNegativeDecDecIndexes_Two(),
                                /*pri*/ TraceIndexMapKey::Order::Desc,
                                /*seci*/ TraceIndexMapKey::Order::Desc,
                                trt.getTA_Two,
                                trt.getPriNDec,
                                trt.getSecNDec,
                                trt.getTO_Two,
                                trt.getStepN(), trt.getStepN(),
                                trt.defaultSampleFormat_);
}

} // namespace segysdk::segyindex::tests
