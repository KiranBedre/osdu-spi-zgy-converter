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

#include "../SegyFormat/External/LoadContextFromJson.h"
#include "../SegyFormat/External/SegyTraceHeaderStringConsts.h"

#include <gtest/gtest.h>

#include <string>

namespace segysdk::segyformat::external::tests
{

int
getUnmappedFields(const std::vector<std::shared_ptr<SegyHeaderField>>& headers)
{
    int unmappedCount = 0;

    for (const auto& fld : headers) {
        if (fld->getKey().rfind("Unmapped") == 0) {
            unmappedCount++;
        }
    }
    return unmappedCount;
}

TEST(SegyFormatUnitTests, padUpMissingFieldsTest_rev0)
{
    const int segyVersion = 0;

    const std::vector traceHeaders{
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::InlineNumber, DataFormat::Integer, 4, 17),
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::CrosslineNumber, DataFormat::Integer, 4, 21),
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::DelayRecordingTime, DataFormat::Integer, 2, 109),
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::SourceCoordinateX, DataFormat::Integer, 4, 185),
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::SourceCoordinateY, DataFormat::Integer, 4, 189),
    };

    EXPECT_FALSE(traceHeaders.empty());

    const auto rev0_merge = LoadContextFromJson::mergeInputHeadersWithStandard(traceHeaders, segyVersion);

    // Expect following fields to be empty:
    // 1) 73 - 76 Source X
    // 2) 77 - 80 Source Y
    // 3) All fields above 180
    EXPECT_TRUE(rev0_merge.size() > traceHeaders.size());

    const int unmappedCount_rev0 = getUnmappedFields(rev0_merge);
    EXPECT_EQ(unmappedCount_rev0, 15);
}

TEST(SegyFormatUnitTests, padUpMissingFieldsTest_rev1)
{
    const int segyVersion = 1;

    const std::vector traceHeaders{
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::InlineNumber, DataFormat::Integer, 4, 17),
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::CrosslineNumber, DataFormat::Integer, 4, 21),
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::DelayRecordingTime, DataFormat::Integer, 2, 109),
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::SourceCoordinateX, DataFormat::Integer, 4, 181),
        std::make_shared<SegyHeaderField>(SegyTraceHeaderStringConsts::SourceCoordinateY, DataFormat::Integer, 4, 185),
    };

    EXPECT_FALSE(traceHeaders.empty());

    const auto rev1_merge = LoadContextFromJson::mergeInputHeadersWithStandard(traceHeaders, segyVersion);

    // Expect following fields to be empty:
    // 1) 73 - 76 Source X
    // 2) 77 - 80 Source Y
    // 3) 193-196 InlineNo
    // 4) 197-200 CrosslineNo
    // 5) 233-236 Unused
    // 6) 137-240 Unused
    EXPECT_TRUE(rev1_merge.size() > traceHeaders.size());

    const int unmappedCount_rev1 = getUnmappedFields(rev1_merge);
    EXPECT_EQ(unmappedCount_rev1, 6);
}

} // namespace segysdk::segyformat::external::tests
