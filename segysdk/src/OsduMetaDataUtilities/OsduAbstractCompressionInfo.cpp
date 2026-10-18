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

#include <utility>

#include "../OsduMetaDataUtilities/OsduAbstractCompressionInfo.h"
#include "../OsduMetaDataUtilities/OsduAbstractCompressionInfoLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractCompressionInfo::OsduAbstractCompressionInfo(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), compressionInfoStruct_(OsduAbstractCompressionInfoStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractCompressionInfo::OsduAbstractCompressionInfo(OsduAbstractCompressionInfoStruct abstractDatasetStruct)
    : OsduMetadataBase(abstractDatasetStruct.getPartition()), compressionInfoStruct_(std::move(abstractDatasetStruct))
{
    populateJsonObject();
}

OsduAbstractCompressionInfo::OsduAbstractCompressionInfo(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), compressionInfoStruct_(OsduAbstractCompressionInfoStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractCompressionInfo::isValidInput() const
{
    return compressionInfoStruct_.isValid();
}

void
OsduAbstractCompressionInfo::initializeJson()
{
    // Do NOT clean entire Json object - it is in OsduDataElement space ...
    removeMember(OsduAbstractCompressionInfoLiterals::LossyCompressionIndicator);
    removeMember(OsduAbstractCompressionInfoLiterals::CompressionMethodTypeID);
    removeMember(OsduAbstractCompressionInfoLiterals::CompressionLevel);
    removeMember(OsduAbstractCompressionInfoLiterals::CompressedByteCount);
    removeMember(OsduAbstractCompressionInfoLiterals::UncompressedByteCount);

}

void
OsduAbstractCompressionInfo::populateJsonObject()
{    
    initializeJson();

    setOptionalParameter(OsduAbstractCompressionInfoLiterals::LossyCompressionIndicator, compressionInfoStruct_.lossyCompressionIndicator);
    setOptionalParameter(OsduAbstractCompressionInfoLiterals::CompressionMethodTypeID, compressionInfoStruct_.compressionMethodTypeID);
    setOptionalParameter(OsduAbstractCompressionInfoLiterals::CompressionLevel, compressionInfoStruct_.compressionLevel);
    setOptionalParameter(OsduAbstractCompressionInfoLiterals::CompressedByteCount, compressionInfoStruct_.compressedByteCount);
    setOptionalParameter(OsduAbstractCompressionInfoLiterals::UncompressedByteCount, compressionInfoStruct_.uncompressedByteCount);
}

void
OsduAbstractCompressionInfo::initializeMetadataStruct()
{
    compressionInfoStruct_ = OsduAbstractCompressionInfoStruct(getOsduPartition());
}

void
OsduAbstractCompressionInfo::populateMetadataStruct()
{
    initializeMetadataStruct();

    compressionInfoStruct_.lossyCompressionIndicator = getOptionalParameter<bool>(OsduAbstractCompressionInfoLiterals::LossyCompressionIndicator);
    compressionInfoStruct_.compressionMethodTypeID = getOptionalParameter<std::string>(OsduAbstractCompressionInfoLiterals::CompressionMethodTypeID);
    compressionInfoStruct_.compressionLevel = getOptionalParameter<double>(OsduAbstractCompressionInfoLiterals::CompressionLevel);
    compressionInfoStruct_.compressedByteCount = getOptionalParameter<std::string>(OsduAbstractCompressionInfoLiterals::CompressedByteCount);
    compressionInfoStruct_.uncompressedByteCount = getOptionalParameter<std::string>(OsduAbstractCompressionInfoLiterals::UncompressedByteCount);
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractCompressionInfo::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractCompressionInfoStruct>(compressionInfoStruct_);
}
} // namespace segysdk::metadata::osdu
