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

#include "../Utilities/Environment.h"
#include "../UtilityLibrary/TmpCleanupFile.h"
#include "../segysdk/SegyServiceSession.h"
#include "../segysdk/SegyServiceSessionFactory.h"

#include "../Utilities/JsonUtilities.h"

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <optional>

namespace segysdk::api::tests
{

class SegyServiceSessionTests : public testing::Test
{
public:
    void SetUp() override
    {
        seismicTraceDataFile_ = makeTmpFile("OSDU/Mocked/SeismicTraceData.json");
        seismicLineGeometryFile_ = makeTmpFile("OSDU/Mocked/File.SeismicLineGeometry.json", seismicTraceDataFile_->getRandomId());
        traceNavigationFile_ = makeTmpFile("OSDU/Mocked/TraceNavigation.json", seismicTraceDataFile_->getRandomId());

        seismicTraceDataFile_->replaceTmpTestDirMarkerWithPath();
        seismicLineGeometryFile_->replaceTmpTestDirMarkerWithPath();

        auto factory = segysdk::api::SegyServiceSessionFactory(seismicTraceDataFile_->getPath());
        serviceSession_ = factory.createSegyServiceSession();
    }

    std::string getTestFilePath(std::string relativePath)
    {
        auto const testdataDir = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
        if (testdataDir.empty()) {
            std::cerr << "Missing environment variable " << segysdk::utilities::EnvironmentVariableStringConsts::SEGY_LOCALTESTDATA_PATH << std::endl;
        }

        return std::filesystem::path(testdataDir).append(relativePath).string();
    }

    std::shared_ptr<TmpCleanupFile> makeTmpFile(std::string sourcePath, std::optional<std::string> id = std::nullopt)
    {
        const auto fullPath = getTestFilePath(sourcePath);
        if (id.has_value()) {
            return std::make_shared<TmpCleanupFile>(fullPath, id.value());
        }
        else {
            return std::make_shared<TmpCleanupFile>(fullPath);
        }
    }

    std::shared_ptr<SegyServiceSession> getServiceSession()
    {
        return serviceSession_;
    }

private:
    std::shared_ptr<TmpCleanupFile> seismicTraceDataFile_;
    std::shared_ptr<TmpCleanupFile> traceNavigationFile_;
    std::shared_ptr<TmpCleanupFile> seismicLineGeometryFile_;

    std::shared_ptr<SegyServiceSession> serviceSession_;
};

TEST_F(SegyServiceSessionTests, getTraceNavigation)
{
    auto serviceSession = getServiceSession();
    ASSERT_FALSE(serviceSession == nullptr);
    auto traceNavigation = serviceSession->getTraceNavigation();

    EXPECT_EQ(traceNavigation.size(), 984);

    auto checkTraceNavigation = [&traceNavigation](size_t elemNumb, TraceNavigationData expected) {
        EXPECT_EQ(traceNavigation[elemNumb].cdp_, expected.cdp_);
        EXPECT_DOUBLE_EQ(traceNavigation[elemNumb].sp_, expected.sp_);
        EXPECT_DOUBLE_EQ(traceNavigation[elemNumb].x_, expected.x_);
        EXPECT_DOUBLE_EQ(traceNavigation[elemNumb].y_, expected.y_);
    };

    checkTraceNavigation(0, {2100, 954.0, 1.8347318118493725, 58.436488771781491});
    checkTraceNavigation(707, {2807, 1307.1404, 1.9103434863392559, 58.436630747694743});
    checkTraceNavigation(983, {3083, 1445.0, 1.9398597934731905, 58.436683094334725});
}

} // namespace segysdk::api::tests
