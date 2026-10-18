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

#pragma once

#include "SegyTestVolumeDefinition.h"

#include "../SegyIndex/SegyIndex.h"
#include "../Utilities/Ensure.h"

#include <memory>
#include <vector>

namespace segysdk::segyindex::tests
{
class SegyIndexWorkflowTests
{

private:
    static std::vector<std::shared_ptr<SegyTestVolumeDefinition>> getTestDataArray(std::string dataPath);
    static std::vector<std::shared_ptr<SegyTestVolumeDefinition>> getTestDataArrayFromJson(const std::string& json);

private:
    static bool hasEnding(std::string const& fullString, std::string const& ending);
    static void runTestForDataSet(const std::shared_ptr<SegyTestVolumeDefinition>& testDataSet, int& errors);
    static std::string getJsonFromInputFile(const std::string& fileName,
                                            const std::string& sd_url,
                                            const std::string& sd_api_key,
                                            std::unique_ptr<segysdk::utilities::auth::TokenSource> sd_tokenSource);
    static Geometry3D getCloudspinGeometry();
    static Geometry3D getGullfaksGeometry();
    static FileLayoutSpecification getCloudspinFileLayout();
    static FileLayoutSpecification getGullfaksFileLayout();
    static bool checkIfGeometryMatches(const Geometry3D& geomA, const Geometry3D& geomB,
                                       const std::string& datasetName, bool skipIsRegularCheck = false);
    static bool checkIfFileLayoutMatches(const FileLayoutSpecification& geomA, const FileLayoutSpecification& geomB,
                                         const std::string& datasetName, bool skipIsRegularCheck = false);
    static bool performTestsOnIndex(const std::shared_ptr<SegyIndex>& testIndex, const std::shared_ptr<SegyTestVolumeDefinition>& testDataSet, int expectedTraceCount, bool skipIsRegularCheck = false);
    static std::vector<IndexField> getTestIndexLocations(const std::shared_ptr<SegyTestVolumeDefinition>& dataSet);

    static bool ValidateCornerPoints(const std::string& dataset, const std::string& cornerName, GridCell cornerA, GridCell cornerB, uint16_t precision);
    static bool ValidateExtent(const std::string& dataset, const std::string& extentName, HorizontalExtent rangeA, HorizontalExtent rangeB);
    static bool ValidateExtent(const std::string& dataset, const std::string& extentName, VerticalExtent rangeA, VerticalExtent rangeB, uint16_t precision);

public:
    static void runAllTests(const std::string& dataPath);
    static void generateDemoWorkflowJson(std::string dataFolderPath);
};

} // namespace segysdk::segyindex::tests
