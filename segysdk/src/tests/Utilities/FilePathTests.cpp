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

#include "../Utilities/StringUtils.h"

#include <gtest/gtest.h>

#include <iostream>
#include <filesystem>
#include <string>

namespace segysdk::utilities::tests
{

std::string
getFileCollectionPathFromOutputFile(std::string zgyFileName)
{
    // Example of expected output file name:
    // "sd://opendes/sntc/volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.44b826ce-facc-4429-82d5-da359cfb4fec.zgy"

    std::replace(zgyFileName.begin(), zgyFileName.end(), '\\', '/');
    auto pos = zgyFileName.find_last_of('/');

    return zgyFileName.substr(0, pos+1);
}

void
OutputComparisons(const std::string& testName, const std::string& pattern, const std::string& expected)
{
    std::cerr << testName  << ": Comparison test: String = [" << pattern << "]  Expected "
              << "[" << expected << "]" << std::endl;
}

TEST(FilePathTests, GetFolderManuallyTest)
{
    const auto fileName = "sd://opendes/sntc/volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.44b826ce-facc-4429-82d5-da359cfb4fec.zgy";

    const auto filePath = getFileCollectionPathFromOutputFile(fileName);

    const std::string testName = "GetFolderManuallyTest";
    OutputComparisons(testName, filePath, "sd://opendes/sntc/volve/");

    ASSERT_TRUE(filePath == "sd://opendes/sntc/volve/");
}

TEST(FilePathTests, GetFilePathTest1)
{
    std::string fileName = "sd://opendes/sntc/volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.44b826ce-facc-4429-82d5-da359cfb4fec.zgy";

    // parent_path stops at first / or \\ change ... 

    const auto filePath = std::filesystem::path(fileName).parent_path().make_preferred().string();

    const std::string testName = "GetFilePathTest1";
#ifdef _MSC_VER
    OutputComparisons(testName, filePath, "sd://opendes/sntc/volve");
    ASSERT_TRUE(filePath == "sd:\\\\opendes\\sntc\\volve");
#else
    OutputComparisons(testName, filePath, "sd://opendes/sntc");
    ASSERT_TRUE(filePath == "sd://opendes/sntc");
#endif
}

TEST(FilePathTests, GetFilePathTest2)
{
    std::string fileName = "sd://opendes/sntc\\volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.44b826ce-facc-4429-82d5-da359cfb4fec.zgy";

     // parent_path stops at first / or \\ change ...
    const auto filePath = std::filesystem::path(fileName).parent_path().make_preferred().string();

    const std::string testName = "GetFilePathTest2";
#ifdef _MSC_VER
    OutputComparisons(testName, filePath, "sd:\\\\opendes\\sntc\\volve");
    ASSERT_TRUE(filePath == "sd:\\\\opendes\\sntc\\volve");
#else
    OutputComparisons(testName, filePath, "sd://opendes");
    ASSERT_TRUE(filePath == "sd://opendes");
#endif
}

TEST(FilePathTests, GetFilePathTest3)
{
    std::string fileName = "sd://opendes\\sntc\\volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.44b826ce-facc-4429-82d5-da359cfb4fec.zgy";

    const auto filePath = std::filesystem::path(fileName).parent_path().make_preferred().string() + "/";

    const std::string testName = "GetFilePathTest3";
#ifdef _MSC_VER
    OutputComparisons(testName, filePath, "sd:\\\\opendes\\sntc\\volve/");
    ASSERT_TRUE(filePath == "sd:\\\\opendes\\sntc\\volve/");
#else
    OutputComparisons(testName, filePath, "sd:/");
    ASSERT_TRUE(filePath == "sd:/");
#endif
}

TEST(FilePathTests, GetFilePathNearImplementationTest)
{
    std::string fileName = "sd://opendes\\sntc/volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.44b826ce-facc-4429-82d5-da359cfb4fec.zgy";

    std::replace(fileName.begin(), fileName.end(), '\\', '/');
    auto dsp_fcp = std::filesystem::path(fileName).parent_path().make_preferred().string() + +"/";

    const std::string testName = "GetFilePathNearImplementationTest";
#ifdef _MSC_VER
    OutputComparisons(testName, dsp_fcp, "sd:\\\\opendes/sntc/volve/");
    ASSERT_TRUE(dsp_fcp == "sd:\\\\opendes\\sntc\\volve/");
#else
    OutputComparisons(testName, dsp_fcp, "sd://opendes/sntc/volve/");
    ASSERT_TRUE(dsp_fcp == "sd://opendes/sntc/volve/");
#endif
}

TEST(FilePathTests, GetFilePathImplementationTest)
{
    std::string  fileName = "sd://opendes\\sntc/volve\\ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.44b826ce-facc-4429-82d5-da359cfb4fec.zgy";

    std::replace(fileName.begin(), fileName.end(), '\\', '/');
    auto dsp_fcp = std::filesystem::path(fileName).remove_filename().string();
    
    const std::string testName = "GetFilePathImplementationTest";
    OutputComparisons(testName, dsp_fcp, "sd://opendes/sntc/volve/");

    ASSERT_TRUE(dsp_fcp== "sd://opendes/sntc/volve/");
}

} // namespace segysdk::utilities::tests
