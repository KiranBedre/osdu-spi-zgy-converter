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

#include "../Utilities/SafeTime.h"

#include <gtest/gtest.h>

TEST(SafeTime, test_safe_gmtime)
{
    std::time_t time{1615743865};
    std::tm tm{};

    segysdk::utilities::safe_gmtime(time, tm);

    EXPECT_EQ(tm.tm_year, 121);
    EXPECT_EQ(tm.tm_yday, 72);
    EXPECT_EQ(tm.tm_mon, 2);
    EXPECT_EQ(tm.tm_mday, 14);
    EXPECT_EQ(tm.tm_wday, 0);

    EXPECT_EQ(tm.tm_hour, 17);
    EXPECT_EQ(tm.tm_min, 44);
    EXPECT_EQ(tm.tm_sec, 25);
}
