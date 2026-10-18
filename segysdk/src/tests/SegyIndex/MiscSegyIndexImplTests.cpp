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

#include "FakeFileManager.h"

#include "../SegyIndex/SegyIndexImpl.h"
#include "../segysdk_details/SegyException.h"

#include <gtest/gtest.h>

using namespace segysdk;
using namespace segysdk::segyindex;
using namespace segysdk::segyindex::tests;

TEST(SegyIndexImplTests, AtLeastOneFileRequired)
{
    EXPECT_THROW(SegyIndexImpl sut(/*segyFilesVector*/ {}, /*segyFileNames*/ {}, fileio::FileReaderOptions::localOnly()), SegyInvalidArgumentException);
}

TEST(SegyIndexImplTests, IndexBaseFileNameDefaultIsTheFirstInputFile)
{
    const SegyIndexImpl sut(/*segyFilesVector*/ {}, {"test1", "test2"}, fileio::FileReaderOptions::localOnly());

    EXPECT_EQ(sut.getIndexBaseFileName(), "test1");
}

TEST(SegyIndexImplTests, IndexBaseFileNameOverride)
{
    const auto fm = std::make_shared<FakeFileManager>();
    const SegyIndexImpl sut(/*segyFilesVector*/ {}, {"test1", "test2"}, fileio::FileReaderOptions::localOnly(), "override");

    EXPECT_EQ(sut.getIndexBaseFileName(), "override");
}
