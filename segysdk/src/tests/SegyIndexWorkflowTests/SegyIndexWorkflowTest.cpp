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

#include "SegyIndexWorkflowTest.h"

#include "../FileIO/CachedFileReader.h"
#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileManager.h"
#include "../SegyIndex/SegyIndexImpl.h"
#include "../Utilities/EnsureWrapper.h"
#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/TokenSourceFactory.h"
#include "../segysdk/SegySessionFactory.h"
#include "../tests/UtilityLibrary/TestBackdoor.h"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>

namespace segysdk::segyindex::tests
{

using namespace utilities;

void
SegyIndexWorkflowTests::runAllTests(const std::string& workflow)
{
    const auto dataSets = getTestDataArrayFromJson(workflow);
    if (dataSets.empty())
        return;

    int errors = 0;
    for (const auto& dataSet : dataSets) {

        runTestForDataSet(dataSet, errors);
        if (errors == 0) {
            std::cout << "Successfully completed testing of data set " + dataSet->getDatasetName() << std::endl;
        }
        else {
            std::cout << "Testing of data set " + dataSet->getDatasetName() << " completed with " << errors << " errors!" << std::endl;
        }
    }
}

void
SegyIndexWorkflowTests::runTestForDataSet(const std::shared_ptr<SegyTestVolumeDefinition>& testDataSet, int& errors)
{
    const std::string fileName = testDataSet->getSegyFile();
    const std::string configFile = testDataSet->getConfigFile();
    std::string sd_url;
    std::string sd_api_key;

    const bool is_CloudSession = fileio::GenericFileManager::isSDDataset(fileName) || fileio::GenericFileManager::isSDDataset(configFile);

    sd_url = is_CloudSession ? Environment::instance().sdUrl() : "";
    sd_api_key = is_CloudSession ? Environment::instance().sdApiKey() : "";

    const std::string json = getJsonFromInputFile(configFile, sd_url, sd_api_key, TokenSourceFactory::getTokenSource());
    const std::vector segyFileList{fileName};

    const api::SegySessionFactory sessionFactory(segyFileList, json);

    std::cout << "Running session1 tests ... " << std::endl;
    const SDCredentials sdCredentials(sd_url, sd_api_key, TokenSourceFactory::getTokenSource());
    const auto session1 = TestBackdoor::getImpl(
        is_CloudSession ? sessionFactory.CreateSegySession(sdCredentials) : sessionFactory.CreateSegySession());

    const int version = session1->getSegyVersion();
    EnsureWrapper::areEqual(version, testDataSet->getSegyVersion(), errors, testDataSet->getDatasetName(), "SegyVersion");

    constexpr bool forceOverwrite = true;
    bool forceGenerateIndex = false;

    std::cout << "Running session1 GenerateIndexFile ... " << std::endl;
    session1->generateIndexFile(forceGenerateIndex, forceOverwrite);

    std::cout << "Running session1 performTestsOnIndex #1 tests ... " << std::endl;
    performTestsOnIndex(TestBackdoor::getImpl(session1->getIndex()), testDataSet, 0);

    std::cout << "Restore saved index and perform repeat run of index unit tests ... " << std::endl;
    session1->loadIndexFile();

    std::cout << "Running session1 performTestsOnIndex #2 after reload tests ... " << std::endl;
    performTestsOnIndex(TestBackdoor::getImpl(session1->getIndex()), testDataSet, 0);

    std::cout << "Running session2 tests ... " << std::endl;
    const auto session2 = is_CloudSession ? sessionFactory.CreateSegySession(sdCredentials) : sessionFactory.CreateSegySession();
    forceGenerateIndex = true;

    std::cout << "Running session2 GenerateIndexFile ... " << std::endl;
    session2->generateIndexFile(forceGenerateIndex, forceOverwrite);

    std::cout << "Running session2 performTestsOnIndex tests ... " << std::endl;
    const bool skipIsRegularCheck = true; // The test of iGeometry3D.IsReguar test will otherwise fail
    performTestsOnIndex(TestBackdoor::getImpl(session2->getIndex()), testDataSet, testDataSet->getIndexedTraceCount(), skipIsRegularCheck);

    std::cout << "All tests passed: " << testDataSet->getSegyFile() << std::endl;
}

std::vector<std::shared_ptr<SegyTestVolumeDefinition>>
SegyIndexWorkflowTests::getTestDataArray(std::string dataPath)
{

    std::vector<std::shared_ptr<SegyTestVolumeDefinition>> testData;
    if (!hasEnding(dataPath, "\\")) {
        dataPath += "\\";
    }

    const auto cloudspin = std::make_shared<SegyTestVolumeDefinition>(
        "Cloudspin",
        dataPath + "Cloudspin\\mig.sgy",
        dataPath + "Cloudspin\\Cloudspin.json",
        1,
        3600,
        59891,
        876 * 4 + 240,
        true, true, true,
        getCloudspinGeometry(),
        getCloudspinFileLayout());

    std::vector<IndexField> locations = getTestIndexLocations(cloudspin);
    for (const auto& loccs : locations)
        cloudspin->addIndexTestLocations(loccs);
    testData.push_back(cloudspin);

    const auto gullfaks = std::make_shared<SegyTestVolumeDefinition>(
        "Gullfaks",
        dataPath + "Gullfaks\\Gullfaks_Depth_Orig_Amp.sgy",
        dataPath + "Gullfaks\\Rev1-Defaults.json",
        0,
        3600,
        112875,
        227 * 4 + 240,
        true, true, true,
        getGullfaksGeometry(),
        getGullfaksFileLayout());

    locations = getTestIndexLocations(gullfaks);
    for (const auto& locgf : locations)
        gullfaks->addIndexTestLocations(locgf);
    testData.push_back(gullfaks);

    return testData;
}

bool
SegyIndexWorkflowTests::hasEnding(std::string const& fullString, std::string const& ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

std::vector<IndexField>
SegyIndexWorkflowTests::getTestIndexLocations(const std::shared_ptr<SegyTestVolumeDefinition>& dataSet)
{
    int errors = 0;
    std::vector<IndexField> locations;

    FileLayoutSpecification const fileLayout = dataSet->getFileLayoutInfo();

    if (!fileLayout.isRegular
        || fileLayout.IExtent.step() == 0
        || fileLayout.JExtent.step() == 0) {
        return locations;
    }

    int const inlineNoOrigin = fileLayout.JExtent.step() > 0
                                   ? fileLayout.JExtent.first()
                                   : fileLayout.JExtent.last();
    int const crosslineNoOrigin = fileLayout.IExtent.step() > 0
                                      ? fileLayout.IExtent.first()
                                      : fileLayout.IExtent.last();
    size_t const offsetOrigin = dataSet->getInitialHeaderOffset();

    locations.emplace_back(0U, inlineNoOrigin, crosslineNoOrigin, offsetOrigin);

    int const inlineNoOpposite = fileLayout.JExtent.step() > 0
                                     ? fileLayout.JExtent.last()
                                     : fileLayout.JExtent.first();
    int const crosslineNoOpposite = fileLayout.IExtent.step() > 0
                                        ? fileLayout.IExtent.last()
                                        : fileLayout.IExtent.first();

    int const inlineCount
        = (fileLayout.JExtent.last() - fileLayout.JExtent.first()) / fileLayout.JExtent.step() + 1;
    int const crosslineCount
        = (fileLayout.IExtent.last() - fileLayout.IExtent.first()) / fileLayout.IExtent.step() + 1;

    int const traceCount = inlineCount * crosslineCount;

    EnsureWrapper::areEqual(traceCount, dataSet->getIndexedTraceCount(), errors, dataSet->getDatasetName(), "GetIndexLocation-TraceCount");

    size_t const offsetOpposite = dataSet->getInitialHeaderOffset() + static_cast<size_t>(traceCount - 1) * dataSet->getTraceSize();
    locations.emplace_back(0U, inlineNoOpposite, crosslineNoOpposite, offsetOpposite);

    return locations;
}

std::string
SegyIndexWorkflowTests::getJsonFromInputFile(const std::string& fileName,
                                             const std::string& sd_url,
                                             const std::string& sd_api_key,
                                             std::unique_ptr<segysdk::utilities::auth::TokenSource> sd_tokenSource)
{
    std::string result = "{}";

    try {

        const SDCredentials sdCredentials{sd_url, sd_api_key, std::move(sd_tokenSource)};
        const CacheSizeOption cacheSize{2 * 1024 * 1024, 32, 1, 1024U * 1024U};

        const fileio::FileReaderOptions readerOptions{sdCredentials, cacheSize};
        const fileio::FileReference fileReference(fileName, readerOptions);

        if (fileio::GenericFileManager::getSize(fileReference) == 0)
            return result;

        auto [success, contents] = fileio::GenericFileManager::readTextFile(fileio::FileManagerParams{fileReference});
        if (success) {
            return contents;
        }
    }
    catch (std::exception& ex) {
        std::cerr << "Error reading context file." << std::endl;
        std::cerr << ex.what() << std::endl;
    }

    return result;
}

bool
SegyIndexWorkflowTests::performTestsOnIndex(const std::shared_ptr<SegyIndex>& testIndex, const std::shared_ptr<SegyTestVolumeDefinition>& testDataSet, int expectedTraceCount, bool skipIsRegularCheck)
{
    int errors = 0;
    EnsureWrapper::areEqual(static_cast<int>(testIndex->getIndexedTraceCount()), expectedTraceCount, errors, testDataSet->getDatasetName(), "performTestsOnIndex-TraceCount");
    EnsureWrapper::areEqual(testIndex->getTraceSize(), testDataSet->getTraceSize(), errors, testDataSet->getDatasetName(), "performTestsOnIndex-TraceSize");

    if (!skipIsRegularCheck) {
        // Indicates geometry not generated ...
        bool const successGeomentry3D = checkIfGeometryMatches(TestBackdoor::getImpl(testIndex)->getGeometry3D(),
                                                               testDataSet->getGeometryInfo(),
                                                               testDataSet->getDatasetName(), skipIsRegularCheck);
        bool const successFileLayout = checkIfFileLayoutMatches(TestBackdoor::getImpl(testIndex)->getFileLayout(),
                                                                testDataSet->getFileLayoutInfo(),
                                                                testDataSet->getDatasetName(), skipIsRegularCheck);
        EnsureWrapper::isTrue(successGeomentry3D and successFileLayout, errors, testDataSet->getDatasetName(), "performTestsOnIndex-checkIfGeometryMatches");
    }

    EnsureWrapper::areEqual(testIndex->hasConstantCoordinateScalar(), testDataSet->hasConstantCoordinateScalar(), errors, testDataSet->getDatasetName(), "performTestsOnIndex-hasConstantCoordinateScalar");
    EnsureWrapper::areEqual(testIndex->hasConstantTraceLength(), testDataSet->hasConstantTraceLength(), errors, testDataSet->getDatasetName(), "performTestsOnIndex-hasConstantTraceLength");
    EnsureWrapper::areEqual(testIndex->hasConstantTraceWeightingFactor(), testDataSet->hasConstantTraceWeightingFactor(), errors, testDataSet->getDatasetName(), "performTestsOnIndex-hasConstantTraceWeightingFactor");

    const std::vector<IndexField> testLocationsVector = testDataSet->getIndexTestLocations();
    for (const auto& testTraceLoc : testLocationsVector) {
        const int myInline = testTraceLoc.getInline();
        const int myCrossline = testTraceLoc.getCrossline();
        const size_t expectedOffset = testTraceLoc.getOffset();

        auto myIndex = testIndex->getIndexAt(myInline, myCrossline);

        EnsureWrapper::areEqual(myIndex.getInline(), myInline, errors, testDataSet->getDatasetName(), "performTestsOnIndex-Inline");
        EnsureWrapper::areEqual(myIndex.getCrossline(), myCrossline, errors, testDataSet->getDatasetName(), "performTestsOnIndex-Crossline");
        EnsureWrapper::areEqual(myIndex.getOffset(), expectedOffset, errors, testDataSet->getDatasetName(), "performTestsOnIndex-Offset");
    }
    if (errors == 0) {
        std::cout << "Successfully completed testing index properties." << std::endl;
        return true;
    }
    else {
        std::cout << "Testeing of index properties finished with " << errors << " errors!" << std::endl;
        return false;
    }
}

Geometry3D
SegyIndexWorkflowTests::getCloudspinGeometry()
{
    Geometry3D geometry3d;

    geometry3d.JVectorExtent = {500, 770, 1};
    geometry3d.IVectorExtent = {360, 580, 1};
    geometry3d.sampleExtent = {0.015, 3.5190, 0.004, segyio::VerticalDomain::Time, segyio::VerticalUOM::seconds};

    geometry3d.isRegular = true;
    geometry3d.isInlineOrdered = true;

    geometry3d.dataGrid.gridCell_11.i = 500;
    geometry3d.dataGrid.gridCell_11.j = 360;
    geometry3d.dataGrid.gridCell_11.location = {598582.00, -170134.00};

    geometry3d.dataGrid.gridCell_1N.i = 500;
    geometry3d.dataGrid.gridCell_1N.j = 580;
    geometry3d.dataGrid.gridCell_1N.location = {1610682.00, -170134.00};

    geometry3d.dataGrid.gridCell_N1.i = 770;
    geometry3d.dataGrid.gridCell_N1.j = 360;
    geometry3d.dataGrid.gridCell_N1.location = {1598582.00, -184984.00};

    geometry3d.dataGrid.gridCell_NN.i = 770;
    geometry3d.dataGrid.gridCell_NN.j = 580;
    geometry3d.dataGrid.gridCell_NN.location = {1610682.00, -184984.00};

    geometry3d.dataGrid.JVectorUnitDistance = 55.000;
    geometry3d.dataGrid.IVectorUnitDistance = 55.000;

    geometry3d.dataGrid.IVectorRotationAngle = 180.00;
    geometry3d.dataGrid.JVectorRotationAngle = 90.00;

    return geometry3d;
}

Geometry3D
SegyIndexWorkflowTests::getGullfaksGeometry()
{
    Geometry3D geometry3d;

    geometry3d.JVectorExtent = {152, 900, 2};
    geometry3d.IVectorExtent = {150, 750, 2};

    geometry3d.sampleExtent = {1.4330, 2.548705, 0.004915, segyio::VerticalDomain::Time, segyio::VerticalUOM::seconds};

    geometry3d.isRegular = true;
    geometry3d.isInlineOrdered = true;

    geometry3d.dataGrid.gridCell_11.i = 152;
    geometry3d.dataGrid.gridCell_11.j = 150;
    geometry3d.dataGrid.gridCell_11.location = {451200.00, 6780362.49};

    geometry3d.dataGrid.gridCell_1N.i = 152;
    geometry3d.dataGrid.gridCell_1N.j = 750;
    geometry3d.dataGrid.gridCell_1N.location = {458700.00, 6780257.99};

    geometry3d.dataGrid.gridCell_N1.i = 900;
    geometry3d.dataGrid.gridCell_N1.j = 150;
    geometry3d.dataGrid.gridCell_N1.location = {451330.00, 6789695.01};

    geometry3d.dataGrid.gridCell_NN.i = 900;
    geometry3d.dataGrid.gridCell_NN.j = 750;
    geometry3d.dataGrid.gridCell_NN.location = {458830.00, 6789590.00};

    geometry3d.dataGrid.JVectorUnitDistance = 24.956;
    geometry3d.dataGrid.IVectorUnitDistance = 25.006;

    geometry3d.dataGrid.IVectorRotationAngle = 0.798;
    geometry3d.dataGrid.JVectorRotationAngle = 90.798;

    return geometry3d;
}

FileLayoutSpecification
SegyIndexWorkflowTests::getCloudspinFileLayout()
{
    FileLayoutSpecification result;

    result.JExtent = {500, 770, 1};
    result.IExtent = {360, 580, 1};
    result.sampleExtent = {0.015, 3.5190, 0.004, segyio::VerticalDomain::Time, segyio::VerticalUOM::seconds};

    result.isRegular = true;
    result.isInlineOrdered = true;

    return result;
}

FileLayoutSpecification
SegyIndexWorkflowTests::getGullfaksFileLayout()
{
    FileLayoutSpecification result;

    result.JExtent = {152, 900, 2};
    result.IExtent = {150, 750, 2};

    result.sampleExtent = {1.4330, 2.548705, 0.004915, segyio::VerticalDomain::Time, segyio::VerticalUOM::seconds};

    result.isRegular = true;
    result.isInlineOrdered = true;

    return result;
}

bool
SegyIndexWorkflowTests::checkIfGeometryMatches(const Geometry3D& geomA, const Geometry3D& geomB, const std::string& dataset, bool skipIsRegularCheck)
{
    int errors = 0;
    const uint16_t precision = 3;

    bool success = ValidateExtent(dataset, "JVectorExtent", geomA.JVectorExtent, geomB.JVectorExtent);
    EnsureWrapper::isTrue(success, errors);

    success = ValidateExtent(dataset, "IVectorExtent", geomA.IVectorExtent, geomB.IVectorExtent);
    EnsureWrapper::isTrue(success, errors);

    success = ValidateExtent(dataset, "SampleExtent", geomA.sampleExtent, geomB.sampleExtent, precision);
    EnsureWrapper::isTrue(success, errors);

    EnsureWrapper::areEqual(geomA.isInlineOrdered, geomB.isInlineOrdered, errors, dataset, "checkIfGeometryMatches-isInlineOrdered");
    if (!skipIsRegularCheck) {
        EnsureWrapper::areEqual(geomA.isRegular, geomB.isRegular, errors, dataset, "checkIfGeometryMatches-isRegular");
    }

    EnsureWrapper::areAlmostEqual(geomA.dataGrid.JVectorRotationAngle, geomB.dataGrid.JVectorRotationAngle, precision, errors, dataset, "checkIfGeometryMatches-JVectorRotationAngle");
    EnsureWrapper::areAlmostEqual(geomA.dataGrid.JVectorUnitDistance, geomB.dataGrid.JVectorUnitDistance, precision, errors, dataset, "checkIfGeometryMatches-JVectorUnitDistance");
    EnsureWrapper::areAlmostEqual(geomA.dataGrid.IVectorRotationAngle, geomB.dataGrid.IVectorRotationAngle, precision, errors, dataset, "checkIfGeometryMatches-IVectorRotationAngle");
    EnsureWrapper::areAlmostEqual(geomA.dataGrid.IVectorUnitDistance, geomB.dataGrid.IVectorUnitDistance, precision, errors, dataset, "checkIfGeometryMatches-IVectorUnitDistance");

    success = ValidateCornerPoints(dataset, "gridCell_11", geomA.dataGrid.gridCell_11, geomB.dataGrid.gridCell_11, precision);
    EnsureWrapper::isTrue(success, errors);

    success = ValidateCornerPoints(dataset, "gridCell_1N", geomA.dataGrid.gridCell_1N, geomB.dataGrid.gridCell_1N, precision);
    EnsureWrapper::isTrue(success, errors);

    success = ValidateCornerPoints(dataset, "gridCell_N1", geomA.dataGrid.gridCell_N1, geomB.dataGrid.gridCell_N1, precision);
    EnsureWrapper::isTrue(success, errors);

    success = ValidateCornerPoints(dataset, "gridCell_NN", geomA.dataGrid.gridCell_NN, geomB.dataGrid.gridCell_NN, precision);
    EnsureWrapper::isTrue(success, errors);

    return errors == 0 ? true : false;
}

bool
SegyIndexWorkflowTests::checkIfFileLayoutMatches(const FileLayoutSpecification& fileLayoutA, const FileLayoutSpecification& fileLayoutB,
                                                 const std::string& dataset, bool skipIsRegularCheck)
{
    int errors = 0;
    const uint16_t precision = 3;

    bool success = ValidateExtent(dataset, "JVectorExtent", fileLayoutA.JExtent, fileLayoutB.JExtent);
    EnsureWrapper::isTrue(success, errors);

    success = ValidateExtent(dataset, "IVectorExtent", fileLayoutA.IExtent, fileLayoutB.IExtent);
    EnsureWrapper::isTrue(success, errors);

    success = ValidateExtent(dataset, "SampleExtent", fileLayoutA.sampleExtent, fileLayoutB.sampleExtent, precision);
    EnsureWrapper::isTrue(success, errors);

    EnsureWrapper::areEqual(fileLayoutA.isInlineOrdered, fileLayoutB.isInlineOrdered, errors, dataset, "checkIfFileLayoutMatches-isInlineOrdered");
    if (!skipIsRegularCheck) {
        EnsureWrapper::areEqual(fileLayoutA.isRegular, fileLayoutB.isRegular, errors, dataset, "checkIfFileLayoutMatches-isRegular");
    }

    return errors == 0 ? true : false;
}

bool
SegyIndexWorkflowTests::ValidateExtent(const std::string& dataset, const std::string& extentName, HorizontalExtent rangeA, HorizontalExtent rangeB)
{

    int errors = 0;
    EnsureWrapper::areEqual(rangeA.first(), rangeB.first(), errors, dataset, "ValidateExtent-" + extentName + ".first");
    EnsureWrapper::areEqual(rangeA.last(), rangeB.last(), errors, dataset, "ValidateExtent-" + extentName + ".last");
    EnsureWrapper::areEqual(rangeA.step(), rangeB.step(), errors, dataset, "ValidateExtent-" + extentName + ".step");

    return errors == 0 ? true : false;
}
bool
SegyIndexWorkflowTests::ValidateExtent(const std::string& dataset, const std::string& extentName, VerticalExtent rangeA, VerticalExtent rangeB, uint16_t precision)
{

    int errors = 0;
    EnsureWrapper::areAlmostEqual(rangeA.first(), rangeB.first(), precision, errors, dataset, "ValidateExtent-" + extentName + ".first");
    EnsureWrapper::areAlmostEqual(rangeA.last(), rangeB.last(), precision, errors, dataset, "ValidateExtent-" + extentName + ".last");
    EnsureWrapper::areAlmostEqual(rangeA.step(), rangeB.step(), precision, errors, dataset, "ValidateExtent-" + extentName + ".step");

    return errors == 0 ? true : false;
}

bool
SegyIndexWorkflowTests::ValidateCornerPoints(const std::string& dataset, const std::string& cornerName, GridCell cornerA, GridCell cornerB, uint16_t precision)
{
    int errors = 0;

    EnsureWrapper::areEqual(cornerA.i, cornerB.i, errors, dataset, "ValidateCornerPoints-" + cornerName + ".i");
    EnsureWrapper::areEqual(cornerA.j, cornerB.j, errors, dataset, "ValidateCornerPoints-" + cornerName + ".j");

    EnsureWrapper::areAlmostEqual(cornerA.location.X, cornerB.location.X, precision, errors, dataset, "ValidateCornerPoints - " + cornerName + ".X");
    EnsureWrapper::areAlmostEqual(cornerA.location.Y, cornerB.location.Y, precision, errors, dataset, "ValidateCornerPoints-" + cornerName + ".Y");
    EnsureWrapper::isTrue(cornerA.location.IsValid(), errors);
    EnsureWrapper::isTrue(cornerB.location.IsValid(), errors);

    return errors == 0 ? true : false;
}

std::vector<std::shared_ptr<SegyTestVolumeDefinition>>
SegyIndexWorkflowTests::getTestDataArrayFromJson(const std::string& workflowFile)
{
    std::vector<std::shared_ptr<SegyTestVolumeDefinition>> workflow;

    if (!std::filesystem::exists(workflowFile)) {
        std::cerr << "Input workflow file " << workflowFile << " does not exist!" << std::endl;
        return workflow;
    }

    try {
        auto [success, workflowJsonStr] = fileio::GenericFileManager::readTextFile(fileio::FileManagerParams{{workflowFile, fileio::FileReaderOptions::localOnly()}});
        if (!success) {
            std::cerr << "Unable to read input workflow json file" << std::endl;
            return workflow;
        }

        Json::Value root = JsonUtilities::getJsonFromString(workflowJsonStr);

        if (!root.isNull() && root.isMember("Workflow")) {
            Json::Value wfArray = root["Workflow"];
            for (Json::Value::ArrayIndex i = 0; i != wfArray.size(); i++) {
                if (!wfArray[i].isMember("DatasetName") || !wfArray[i].isMember("SegyFile") || !wfArray[i].isMember("ConfigFile")
                    || !wfArray[i].isMember("Geometry3D") || !wfArray[i].isMember("FileLayoutSpecification")) {
                    std::cout << "Workflow json must contain definition for all defined atttributes, e.g. DatasetName, SegyFile, ConfigFile, Geometry3D and FileLayoutSpecification - skipping workflow step ..." << std::endl;
                    continue;
                }

                std::string datasetName;
                std::string segyFile;
                std::string configFile;
                int segyVersion = 0;
                size_t initialHeaderOffset = 0;
                int indexedTraceCount = 0;
                size_t traceSize = 0;
                bool hasConstantCoordScalar = false;
                bool hasConstantTraceLength = false;
                bool hasConstantTWF = false;
                Geometry3D geometry3D;
                FileLayoutSpecification fileLayout;
                std::vector<IndexField> indexLocations;

                // We have established that these first 4 attributes exist ...
                datasetName = wfArray[i]["DatasetName"].asString();
                segyFile = wfArray[i]["SegyFile"].asString();
                configFile = wfArray[i]["ConfigFile"].asString();
                geometry3D = SegyIndexImpl::getGeometry3DStructFromJson(wfArray[i]["Geometry3D"]);
                fileLayout = SegyIndexImpl::getFileLayoutStructFromJson(wfArray[i]["FileLayoutSpecification"]);

                if (wfArray[i].isMember("FirstTraceOffset")) {
                    initialHeaderOffset = wfArray[i]["FirstTraceOffset"].asUInt();
                }
                if (wfArray[i].isMember("IndexedTraceCount")) {
                    indexedTraceCount = wfArray[i]["IndexedTraceCount"].asInt();
                }
                if (wfArray[i].isMember("TraceSize")) {
                    traceSize = wfArray[i]["TraceSize"].asUInt();
                }
                if (wfArray[i].isMember("SegyVersion")) {
                    segyVersion = wfArray[i]["SegyVersion"].asInt();
                }
                if (wfArray[i].isMember("ConstantCoordScalar")) {
                    hasConstantCoordScalar = wfArray[i]["ConstantCoordScalar"].asBool();
                }
                if (wfArray[i].isMember("ConstantTraceLength")) {
                    hasConstantTraceLength = wfArray[i]["ConstantTraceLength"].asBool();
                }
                if (wfArray[i].isMember("ConstantTraceWeightingFactor")) {
                    hasConstantTWF = wfArray[i]["ConstantTraceWeightingFactor"].asBool();
                }

                auto dataset = std::make_shared<SegyTestVolumeDefinition>(datasetName, segyFile, configFile, segyVersion, initialHeaderOffset, indexedTraceCount, traceSize,
                                                                          hasConstantCoordScalar, hasConstantTraceLength, hasConstantTWF,
                                                                          geometry3D, fileLayout);
                workflow.push_back(dataset);
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "Failed to parse input json workflow file. \nException: " << ex.what() << std::endl;
    }
    return workflow;
}

void
SegyIndexWorkflowTests::generateDemoWorkflowJson(std::string dataFolderPath)
{

    auto dataSets = getTestDataArray(std::move(dataFolderPath));

    Json::Value root;

    Json::Value datasetArray;
    for (const auto& dataSet : dataSets) {

        Json::Value datasetJson;
        datasetJson["DatasetName"] = dataSet->getDatasetName();
        datasetJson["SegyFile"] = dataSet->getSegyFile();
        datasetJson["ConfigFile"] = dataSet->getConfigFile();
        datasetJson["SegyVersion"] = dataSet->getSegyVersion();
        datasetJson["FirstTraceOffset"] = static_cast<Json::UInt64>(dataSet->getInitialHeaderOffset());
        datasetJson["IndexedTraceCount"] = dataSet->getIndexedTraceCount();
        datasetJson["TraceSize"] = static_cast<Json::UInt64>(dataSet->getTraceSize());
        datasetJson["ConstantCoordScalar"] = dataSet->hasConstantCoordinateScalar();
        datasetJson["ConstantTraceLength"] = dataSet->hasConstantTraceLength();
        datasetJson["ConstantTraceWeightingFactor"] = dataSet->hasConstantTraceWeightingFactor();
        datasetJson["Geometry3D"] = SegyIndexImpl::getGeometry3DAsJson(dataSet->getGeometryInfo());
        datasetJson["FileLayoutSpecification"] = SegyIndexImpl::getFileLayoutAsJson(dataSet->getFileLayoutInfo());

        datasetArray.append(datasetJson);
    }

    root["Workflow"] = datasetArray;

    std::string workflowFileName = "demoIndexTestWorkflow.json";

    try {
        std::ofstream out(workflowFileName);
        out << root.toStyledString();
        out.close();
        std::cout << "Demo workflow written out to " + workflowFileName << std::endl;
    }
    catch (...) {
        std::cerr << "Error writing demo workflow to file " << workflowFileName << std::endl;
    }
}

} // namespace segysdk::segyindex::tests
