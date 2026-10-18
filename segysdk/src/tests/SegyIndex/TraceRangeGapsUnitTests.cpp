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

class TraceRangeGapsTest : public testing::Test
{
    /* EMPTY */
};

// Developer note:
// The BASIC tests can be temporarily disabled in case a test fails for reasons outwith the responsibility of this code.
// To disable the tests, simply add the prefix "DISABLED_" to the test name. e.g.
// To reactivate, just remove the "DISABLED_" prefix again.
// TEST_F(TraceRangeGapsTest, DetectTraceRangeMapGapsOne) => TEST_F(TraceRangeGapsTest, DISABLED_DetectTraceRangeMapGapsOne)

TEST_F(TraceRangeGapsTest, DetectTraceRangeMapGapsOne)
{
    TraceRangeTester trt;

    trt.testDetectTraceRangeMapGaps(trt.getTisRangesAvailability_One(),
                                    trt.getTisRangesAbsence_One(),
                                    trt.getRA_One,
                                    trt.getPriPInc,
                                    trt.getSecPInc,
                                    trt.getStepP(),
                                    trt.sizeOfMissingRanges_One_,
                                    trt.numberOfLiveTraces_One_);
}

TEST_F(TraceRangeGapsTest, DetectTraceRangeMapGapsTwo)
{
    TraceRangeTester trt;

    trt.testDetectTraceRangeMapGaps(trt.getTisRangesAvailability_Two(),
                                    trt.getTisRangesAbsence_Two(),
                                    trt.getRA_Two,
                                    trt.getPriPInc,
                                    trt.getSecPInc,
                                    trt.getStepP(),
                                    trt.sizeOfMissingRanges_Two_,
                                    trt.numberOfLiveTraces_Two_);
}

TEST_F(TraceRangeGapsTest, DetectTraceRangeMapGapsThree)
{
    TraceRangeTester trt;

    trt.testDetectTraceRangeMapGaps(trt.getTisRangesAvailability_Three(),
                                    trt.getTisRangesAbsence_Three(),
                                    trt.getRA_Three,
                                    trt.getPriPInc,
                                    trt.getSecPInc,
                                    trt.getStepP(),
                                    trt.sizeOfMissingRanges_Three_,
                                    trt.numberOfLiveTraces_Three_);
}

TEST_F(TraceRangeGapsTest, DetectTraceRangeMapGapsFour)
{
    TraceRangeTester trt;

    trt.testDetectTraceRangeMapGaps(trt.getTisRangesAvailability_Four(),
                                    trt.getTisRangesAbsence_Four(),
                                    trt.getRA_Four,
                                    trt.getPriPInc,
                                    trt.getSecPInc,
                                    trt.getStepP(),
                                    trt.sizeOfMissingRanges_Four_,
                                    trt.numberOfLiveTraces_Four_);
}

} // namespace segysdk::segyindex::tests
