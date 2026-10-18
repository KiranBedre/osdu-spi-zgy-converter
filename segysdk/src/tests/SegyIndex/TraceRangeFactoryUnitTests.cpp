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

#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{
class TraceRangeFactoryTest : public testing::Test
{
    /* EMPTY */
};

// Developer note:
// The BASIC tests can be temporarily disabled in case a test fails for reasons outwith the responsibility of this code.
// To disable the tests, simply add the prefix "DISABLED_" to the test name. e.g.
// To reactivate, just remove the "DISABLED_" prefix again.
// TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveIncIncIndexesOne) => TEST_F(TraceRangeFactoryTest, DISABLED_FillTraceStorage_PositiveIncIncIndexesOne)

TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveIncIncIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisPositiveIncIncIndexes_One(),
                             trt.getCollatedData_One,
                             trt.getIBD_One,
                             trt.getPriPInc,
                             trt.getSecPInc,
                             trt.getStepP(), trt.getStepP(),
                             trt.sizeOfIbd_One_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveIncIncIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisPositiveIncIncIndexes_Two(),
                             trt.getCollatedData_Two,
                             trt.getIBD_Two,
                             trt.getPriPInc,
                             trt.getSecPInc,
                             trt.getStepP(), trt.getStepP(),
                             trt.sizeOfIbd_Two_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveIncDecIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisPositiveIncDecIndexes_One(),
                             trt.getCollatedData_One,
                             trt.getIBD_One,
                             trt.getPriPInc,
                             trt.getSecPDec,
                             trt.getStepP(), trt.getStepN(),
                             trt.sizeOfIbd_One_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveIncDecIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisPositiveIncDecIndexes_Two(),
                             trt.getCollatedData_Two,
                             trt.getIBD_Two,
                             trt.getPriPInc,
                             trt.getSecPDec,
                             trt.getStepP(), trt.getStepN(),
                             trt.sizeOfIbd_Two_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveDecIncIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisPositiveDecIncIndexes_One(),
                             trt.getCollatedData_One,
                             trt.getIBD_One,
                             trt.getPriPDec,
                             trt.getSecPInc,
                             trt.getStepN(), trt.getStepP(),
                             trt.sizeOfIbd_One_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveDecIncIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisPositiveDecIncIndexes_Two(),
                             trt.getCollatedData_Two,
                             trt.getIBD_Two,
                             trt.getPriPDec,
                             trt.getSecPInc,
                             trt.getStepN(), trt.getStepP(),
                             trt.sizeOfIbd_Two_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveDecDecIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisPositiveDecDecIndexes_One(),
                             trt.getCollatedData_One,
                             trt.getIBD_One,
                             trt.getPriPDec,
                             trt.getSecPDec,
                             trt.getStepN(), trt.getStepN(),
                             trt.sizeOfIbd_One_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_PositiveDecDecIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisPositiveDecDecIndexes_Two(),
                             trt.getCollatedData_Two,
                             trt.getIBD_Two,
                             trt.getPriPDec,
                             trt.getSecPDec,
                             trt.getStepN(), trt.getStepN(),
                             trt.sizeOfIbd_Two_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_NegativeIncIncIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisNegativeIncIncIndexes_One(),
                             trt.getCollatedData_One,
                             trt.getIBD_One,
                             trt.getPriNInc,
                             trt.getSecNInc,
                             trt.getStepP(), trt.getStepP(),
                             trt.sizeOfIbd_One_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_NegativeIncIncIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisNegativeIncIncIndexes_Two(),
                             trt.getCollatedData_Two,
                             trt.getIBD_Two,
                             trt.getPriNInc,
                             trt.getSecNInc,
                             trt.getStepP(), trt.getStepP(),
                             trt.sizeOfIbd_Two_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_NegativeIncDecIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisNegativeIncDecIndexes_One(),
                             trt.getCollatedData_One,
                             trt.getIBD_One,
                             trt.getPriNInc,
                             trt.getSecNDec,
                             trt.getStepP(), trt.getStepN(),
                             trt.sizeOfIbd_One_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_NegativeIncDecIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisNegativeIncDecIndexes_Two(),
                             trt.getCollatedData_Two,
                             trt.getIBD_Two,
                             trt.getPriNInc,
                             trt.getSecNDec,
                             trt.getStepP(), trt.getStepN(),
                             trt.sizeOfIbd_Two_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_NegativeDecIncIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisNegativeDecIncIndexes_One(),
                             trt.getCollatedData_One,
                             trt.getIBD_One,
                             trt.getPriNDec,
                             trt.getSecNInc,
                             trt.getStepN(), trt.getStepP(),
                             trt.sizeOfIbd_One_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_NegativeDecIncIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisNegativeDecIncIndexes_Two(),
                             trt.getCollatedData_Two,
                             trt.getIBD_Two,
                             trt.getPriNDec,
                             trt.getSecNInc,
                             trt.getStepN(), trt.getStepP(),
                             trt.sizeOfIbd_Two_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_NegativeDecDecIndexesOne)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisNegativeDecDecIndexes_One(),
                             trt.getCollatedData_One,
                             trt.getIBD_One,
                             trt.getPriNDec,
                             trt.getSecNDec,
                             trt.getStepN(), trt.getStepN(),
                             trt.sizeOfIbd_One_);
}

TEST_F(TraceRangeFactoryTest, FillTraceStorage_NegativeDecDecIndexesTwo)
{
    TraceRangeTester trt;

    trt.testFillTraceStorage(trt.getTisNegativeDecDecIndexes_Two(),
                             trt.getCollatedData_Two,
                             trt.getIBD_Two,
                             trt.getPriNDec,
                             trt.getSecNDec,
                             trt.getStepN(), trt.getStepN(),
                             trt.sizeOfIbd_Two_);
}

} // namespace segysdk::segyindex::tests
