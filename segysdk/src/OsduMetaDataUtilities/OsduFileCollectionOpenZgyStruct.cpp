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

#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractDatasetLiterals.h"

#include <filesystem>


namespace segysdk::metadata::osdu
{
OsduFileCollectionOpenZgyStruct::OsduFileCollectionOpenZgyStruct(const std::string& osduPartition)
  : OsduMetadataStructBase(osduPartition),
    commonMetadataStruct(osduPartition),
    helper_(OsduMetadataHelper(getPartition()))
{
    /* Empty */
}

bool
OsduFileCollectionOpenZgyStruct::isValid() const
{
    return structValid(commonMetadataStruct) and structValid(dataStruct);
}

std::vector<std::string>
OsduFileCollectionOpenZgyStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduFileCollectionOpenZgyStruct::isValid() failed.");
    }

    performStructValidityScan(results, commonMetadataStruct);
    performStructValidityScan(results, dataStruct);

    return results;
}

void
OsduFileCollectionOpenZgyStruct::propagatePartition(const std::string& partition)
{
    commonMetadataStruct.setPartition(partition);

    if (dataStruct.has_value()) {
        dataStruct->setPartition(partition);
    }
}

void
OsduFileCollectionOpenZgyStruct::addFileProperties(const std::string& zgyFileName, uint64_t fileSize)
{

    std::string fileName = zgyFileName;

    std::replace(fileName.begin(), fileName.end(), '\\', '/');
    auto dsp_fcp = std::filesystem::path(fileName).remove_filename().string();
    if (!dataStruct.has_value()) {
        dataStruct = OsduFileCollectionOpenZgyDataStruct(getPartition());
    }

    if (!dataStruct.value().datasetStruct.has_value()) {
        dataStruct.value().datasetStruct = OsduAbstractDatasetStruct(getPartition());
    }
    dataStruct.value().datasetStruct.value().totalSize = std::to_string(fileSize);
    dataStruct.value().datasetStruct.value().endian = OsduAbstractDatasetLiterals::LittleEndian;

    auto fileCollectionStruct = dataStruct.value().fileCollectionStruct.has_value() ? dataStruct.value().fileCollectionStruct.value() : segysdk::metadata::osdu::OsduAbstractFileCollectionStruct(getPartition());

    segysdk::metadata::osdu::OsduAbstractFileSourceInfoStruct fileSource(getPartition());
    fileSource.fileSize = std::to_string(fileSize);
    fileSource.fileSource = fileName;
    fileCollectionStruct.fileCollectionPath = dsp_fcp;
    fileCollectionStruct.fileSourceInfoStructs = {fileSource};

    dataStruct.value().fileCollectionStruct = fileCollectionStruct;
}

void
OsduFileCollectionOpenZgyStruct::addPrecision(OsduAbstractSeismicPrecisionStruct precisionStruct)
{
    if (!dataStruct.has_value()) {
        dataStruct = OsduFileCollectionOpenZgyDataStruct(getPartition());
    }

    dataStruct.value().precisionStruct = precisionStruct;   
}

void
OsduFileCollectionOpenZgyStruct::addCompressionInfo(OsduAbstractCompressionInfoStruct compressionInfoStruct)
{
    if (!dataStruct.has_value()) {
        dataStruct = OsduFileCollectionOpenZgyDataStruct(getPartition());
    }

    dataStruct.value().compressionInfoStruct = compressionInfoStruct;
}

} // namespace segysdk::metadata::osdu
