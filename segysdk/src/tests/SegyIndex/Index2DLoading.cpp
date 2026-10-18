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

#include "../segysdk/SegySessionFactory.h"
#include "../tests/UtilityLibrary/AutoCleanupTestFile.h"

#include <gtest/gtest.h>

#include <cmath>
#include <cstring>

namespace segysdk::segyindex::tests
{

TEST(Segy2DLoading, test_save_load_parameters)
{
    const bool is2D = true;
    AutoCleanupTestFile tempFile("small2D.sgy");
    // User sets whether data is 2D or not
    tempFile.create_index(is2D);
    auto segyFileName = tempFile.get_path();
    auto jsonConfig = tempFile.get_segy_config_json_string();

    auto session = segysdk::api::SegySessionFactory({segyFileName}, jsonConfig).CreateSegySession();

    auto index = session->getIndex();

    ASSERT_FALSE(index->isSegy3D());

    EXPECT_TRUE(index->isRegular());

    size_t expectedOffset = 0;
    EXPECT_EQ(index->getOffsetForCMP(10), expectedOffset);
    EXPECT_EQ(index->getOffsetForTraceNo(1), expectedOffset);

    expectedOffset = 3600;
    EXPECT_EQ(index->getOffsetForCMP(501), expectedOffset);
    EXPECT_EQ(index->getOffsetForTraceNo(300), expectedOffset);

    expectedOffset = 176256;
    EXPECT_EQ(index->getOffsetForCMP(600), expectedOffset);
    EXPECT_EQ(index->getOffsetForTraceNo(399), expectedOffset);

    const size_t cmpLast = 701;
    const size_t traceLast = 500;
    expectedOffset = 352400;
    EXPECT_EQ(index->getOffsetForCMP(cmpLast), expectedOffset);
    EXPECT_EQ(index->getOffsetForTraceNo(traceLast), expectedOffset);

    EXPECT_EQ(index->getOffsetForCMP(cmpLast + 1), 0);
    EXPECT_EQ(index->getOffsetForTraceNo(traceLast + 1), 0);
}

} // namespace segysdk::segyindex::tests