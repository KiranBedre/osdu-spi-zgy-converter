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

#include "../Utilities/osdu/GeneratedOutputFileName.h"

#include <gtest/gtest.h>

namespace segysdk::utilities::tests
{

TEST(GeneratedFileNameTests, Nametests)
{
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("\\root\\dir1\\dir2\\dir3\\test.sgy", "").fileName, "\\root\\dir1\\dir2\\dir3\\test.zgy");

    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.sgy", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.sGy", "").fileName, "test.zgy");

    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.SgY", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.sgy", "suffix").fileName, "test.suffix.zgy");

    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.segy", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.sEgY", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.segy", "suffix").fileName, "test.suffix.zgy");

    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test", "").fileName, "test.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test", "suffix").fileName, "test.suffix.zgy");

    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.someext", "").fileName, "test.someext.zgy");
    EXPECT_EQ(segysdk::utilities::osdu::GeneratedOutputFileName("test.someext", "suffix").fileName, "test.someext.suffix.zgy");
}


} // namespace segysdk::utilities::tests
