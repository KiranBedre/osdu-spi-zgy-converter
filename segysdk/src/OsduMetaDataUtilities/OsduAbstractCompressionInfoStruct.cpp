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

#include "../OsduMetaDataUtilities/OsduAbstractCompressionInfoStruct.h"


namespace segysdk::metadata::osdu
{
OsduAbstractCompressionInfoStruct::OsduAbstractCompressionInfoStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduAbstractCompressionInfoStruct::compressionMethodTypeIdValid() const
{
    return !compressionMethodTypeID.has_value() or isValidOSDUReferenceIdString(compressionMethodTypeID.value());
}

bool
OsduAbstractCompressionInfoStruct::isValid() const
{
    return  compressionMethodTypeIdValid();
}

std::vector<std::string>
OsduAbstractCompressionInfoStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduAbstractCompressionInfoStruct::isValid() failed.");
    }

    if (!compressionMethodTypeIdValid()) {
        results.emplace_back("compressionMethodTypeID is not a valid OSDU reference id.");
    }


    return results;
}

void 
OsduAbstractCompressionInfoStruct::setCompressionInfo(segysdk::utilities::ZgyOutputParams zgyOutputParameters, size_t fileSize)
{
    if (!zgyOutputParameters.UseCompression()) {
        lossyCompressionIndicator = false;
        compressionMethodTypeID = std::nullopt;
        compressionLevel = 0;
        uncompressedByteCount = std::to_string(fileSize);   
        return;
    }
    lossyCompressionIndicator = true;
    compressionLevel = zgyOutputParameters.getZgyCompressionSnR();
    compressedByteCount = std::to_string(fileSize);
}

} // namespace segysdk::metadata::osdu
