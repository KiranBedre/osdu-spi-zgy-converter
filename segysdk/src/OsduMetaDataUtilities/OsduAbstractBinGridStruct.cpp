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

#include "../OsduMetaDataUtilities/OsduAbstractBinGridStruct.h"
namespace segysdk::metadata::osdu
{
OsduAbstractBinGridStruct::OsduAbstractBinGridStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

bool
OsduAbstractBinGridStruct::isValid() const
{
    return isValidIdString(typeId, IdStringCheckType::Reference) and isValidIdString(definitionMethodTypeId, IdStringCheckType::Reference)
           and structValid(abcdSpatialLocationStruct);
}

bool 
OsduAbstractBinGridStruct::isCompleteP6Definition() const
{
    return p6BinGridOriginI.has_value()
           and p6BinGridOriginJ.has_value()
           and p6BinGridOriginEasting.has_value()
           and p6BinGridOriginNorthing.has_value()
           and p6BinNodeIncrementOnIaxis.has_value()
           and p6BinNodeIncrementOnJaxis.has_value()
           and p6BinWidthOnIaxis.has_value()
           and p6BinWidthOnJaxis.has_value()
           and p6TransformationMethod.has_value();
}

std::vector<std::string>
OsduAbstractBinGridStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduAbstractBinGridStruct::isValid() failed.");
    }

    performIdStringValidityScan(results, typeId, "OsduAbstractBinGridStruct:typeId: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, definitionMethodTypeId, "OsduAbstractBinGridStruct:definitionMethodTypeId: ", IdStringCheckType::Reference);
    performStructValidityScan(results, abcdSpatialLocationStruct);

    return results;
}

void
OsduAbstractBinGridStruct::propagatePartition(const std::string& partition)
{
    if (abcdSpatialLocationStruct.has_value()) {
        abcdSpatialLocationStruct.value().setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
