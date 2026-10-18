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

#include "OptionsOsdu.h"

#include "GuidGenerator.h"
#include "JsonBinGrid.h"
#include "StorageSvcClient.h"

#include <iostream>

#include "../Logger/Logger.h"
#include "../Utilities/Authorization.h"
#include "../Utilities/Environment.h"
#include "../Utilities/osdu/OsduFileCollectionSegy100.h"
#include "../Utilities/osdu/OsduObject.h"
#include "../Utilities/osdu/OsduSeismicTraceDataFactory.h"
#include "../Utilities/osdu/OsduWorkProduct100.h"
#include "../Utilities/osdu/WellKnownTypes.h"

namespace segysdk::utilities
{

static auto
findSeismicTraceDataWpc(const segysdk::utilities::osdu::OsduWorkProduct100& workProduct, const segysdk::utilities::StorageSvcClient& storageSvcClient)
{
    for (const auto& componentId : workProduct.componentIds()) {
        SEGYLOG_DEBUG("Fetching work product component [" + componentId + "]");
        const auto json = storageSvcClient.getRecordAsJson(componentId);
        const auto objectKind = segysdk::utilities::osdu::OsduObject(json).kind();
        SEGYLOG_DEBUG("Work product component [" + componentId + "] kind: [" + objectKind.kind() + "]");
        if (objectKind.shortTypeName() == segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData) {
            return segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(json);
        }
    }
    throw std::runtime_error("Work product does not contain a SeismicTraceData component.");
}

OptionsOsdu::Args::Args(int argc, const char** argv, const segysdk::utilities::StorageSvcClient& storageSvcClient)
{
    if (argc != 4) {
        help(argv[0]);
        exit(1);
    }

    fileCollectionId_ = argv[2];
    const std::string workProductId = argv[3];
    SEGYLOG_DEBUG("Fetching work product [" + workProductId + "]");
    const segysdk::utilities::osdu::OsduWorkProduct100 workProductJson(storageSvcClient.getRecordAsJson(workProductId));
    const auto seismicTraceData = findSeismicTraceDataWpc(workProductJson, storageSvcClient);
    seismicTraceDataId_ = seismicTraceData.id();
    SEGYLOG_DEBUG("Seismic trace data ID: [" + seismicTraceDataId_ + "]");
    binGridId_ = seismicTraceData.binGridId();
    SEGYLOG_DEBUG("Using bin grid ID: [" + binGridId_ + "] from seismic trace data");
}

std::string
OptionsOsdu::Args::fileCollectionId() const
{
    return fileCollectionId_;
}

std::string
OptionsOsdu::Args::seismicTraceDataId() const
{
    return seismicTraceDataId_;
}

std::string
OptionsOsdu::Args::binGridId() const
{
    return binGridId_;
}

void
OptionsOsdu::Args::help(const std::string& programName)
{
    std::cerr << "Usage: " << programName << " --osdu filecollection_segy_id work_product_id" << std::endl;
}

OptionsOsdu::OptionsOsdu(int argc, const char** argv, const OptionsFileType oft)
  : Options(oft),
    storageSvcClient_(segysdk::utilities::Authorization::storageSvcTokenProvider()),
    args_(argc, argv, storageSvcClient_),
    runId_(segysdk::utilities::GuidGenerator::newGuid()),
    fileCollection_(storageSvcClient_.getRecordAsJson(args_.fileCollectionId())),
    binGridContent_(storageSvcClient_.getRecord(args_.binGridId()))
{
    SEGYLOG_DEBUG("Run ID: " + runId_);
}

std::vector<std::string>
OptionsOsdu::sourceFiles() const
{
    return fileCollection_.segyFileNames();
}

std::string
OptionsOsdu::segyConfig() const
{
    return fileCollection_.content();
}

segysdk::utilities::BinGrid
OptionsOsdu::binGrid() const
{
    return segysdk::utilities::JsonBinGrid::loadBinGridFromContent(binGridContent_);
}

std::string
OptionsOsdu::targetFile() const
{
    return segysdk::utilities::Environment::instance().outputPathOverrideOrDefault(fileCollection_.desiredZgyFileName(runId_));
}

std::string
OptionsOsdu::getFileCollectionId() const
{
    return args_.fileCollectionId();
}

void
OptionsOsdu::finalize() const
{
    // Should this be where the index gets registered?
    // Code below has been deliberately commented out for now.

    /*
    const OpenFileProperties fp(targetFile(), *this);
    const osdu::OsduFileCollectionSegy100 segy(fileCollection_, fp);
    const auto segyIndexId = storageSvcClient_.storeRecord(segy.content());
    auto std = osdu::OsduSeismicTraceData100(storageSvcClient_.getRecordAsJson(args_.seismicTraceDataId()));
    std.addCompletedOpenZGYDataset(segyIndexId, segy.kind());
    storageSvcClient_.storeRecord(std.content());
    */
}

} // namespace segysdk::utilities
