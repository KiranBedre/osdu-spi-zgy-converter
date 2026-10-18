/*
Copyright 2023 Schlumberger

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

#include "../segysdk_details/DLL_EXPORT.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"

#include <string>
#include <optional>
#include <vector>

namespace segysdk::metadata::osdu
{


class SEGYSDK_DLL_PUBLIC OsduMetadataHelper
{

public:
    OsduMetadataHelper();

    /// \brief The OsduMetadataHelper is aiming to help populating osdu reference values
    /// \param osduPartition can be either "osdu" for static reference values, or e.g. the current user partition_ for customizable reference data
    explicit OsduMetadataHelper(std::string osduPartition);

public:
    void setOsduPartition(const std::string& osduPartition);

    /// \brief Reference string is used in queries or reference lookup methods for storage system records or osdu static references. Should have a trailing colon.
    /// \param referencePath   path to the reference value,  e.g. reference-data--EncodingFormatType for customizable reference data
    /// \param referenceValue  This is the actual Id value of the reference item
    /// \param version          This is a string version indicator, if left empty, the latest version will be selected
    /// \return                 string with the correct search id-string
    std::string osduReferenceLookupString(const std::string& referencePath, const std::string& referenceValue, const std::string& version = "") const;

    /// \brief Reference string is used in queries or reference lookup methods for storage system records or osdu static references. Should have a trailing colon.
    /// \param storageId        StorageId to be converted to a reference id used in look-ups
    /// \param version          storage item version to be converted to a reference id used in look-ups
    /// \return                 string with the correct search id-string
    static std::string osduReferenceLookupString(const std::string& storageId, int64_t version);

    /// \brief Reference string is used in queries or reference lookup methods for storage system records or osdu static references.
    /// \param storageId        StorageId to be converted to a reference id used in look-ups
    /// \return                 string with the correct search id-string
    std::string osduReferenceLookupString(const std::string& storageId) const;


    /// \brief The osduStorageSystemIdString is intended to be used ias the value for the "id" field in the metadata file. Should NOT have a trailing colon. Use the osduReferenceLookupString in queries or lookup methods
    /// \param id_class     the class of the item, e.g. "work-product-component--SeismicBinGrid" or "dataset--FileCollection.SEGY"
    /// \param id_value     This is the  actual Id value of the item
    /// \return             string with the correct item id value
    std::string osduStorageSystemIdString(const std::string& id_class, const std::string& id_value) const;

    /// \brief Reference string is used in queries for Osdu static references.
    /// \param referencePath   path to the reference value, e.g. "wks" for static references
    /// \param referenceValue  This is the actual Id value of the reference item
    /// \param version          This is a string version indicator, if left empty, the latest version will be selected
    /// \return                 string with the correct search id-string
    static std::string osduWksReferenceLookupString(const std::string& referenceValue, const std::string& version = "");

    /// \brief Reference string is used in queries for Osdu static references.
    /// \param referenceType    Type of reference, "reference-data--UnitOfMeasure" for units
    /// \param unit             This is the actual string value of the referenced unit
    /// \return                 string with the correct search id-string
    std::string osduUnitReferenceString(const std::string& unit, const std::string& referenceType = "reference-data--UnitOfMeasure") const;

    static std::string osdu_version(int major, int minor = 0, int incremental = 0);
    static std::string osdu_version();

    bool isValidOSDUReferenceIdString(const std::string& attributeValue) const;

    bool isValidOSDUStorageSystemIdString(const std::string& idValue) const;

    bool validPartitionCheck(const std::string& partition) const;

    static std::vector<std::string> parseReferenceAttribute(const std::string& osduReference);

    /// \brief template method to perform validity checks on optional objects
    /// \param testVar variable to check
    /// \param isSystemId boolean value for a string input value that is to be tested as a systemId rather than a reference value
    /// \return true if object is valid, otherwise false
    bool isValidOptionalObject(const std::optional<std::string>& testVar, const bool isSystemId = false) const
    {
        if (!testVar.has_value()) {
            return true;
        }
        return isSystemId ? isValidOSDUStorageSystemIdString(testVar.value()) : isValidOSDUReferenceIdString(testVar.value());
    }

    /// \brief template method to perform validity checks on optional objects
    /// \param testVar variable to check (Not optional)
    /// \param isSystemId boolean value for a string input value that is to be tested as a systemId rather than a reference value
    /// \return true if object is valid, otherwise false
    template<typename T>
    bool isValidObject(T testVar, const bool isSystemId = false) const
    {
        if (!testVar) {
            return true;
        }
        if constexpr (std::is_same_v<T, std::string>) {
            return isSystemId ? isValidOSDUStorageSystemIdString(testVar) : isValidOSDUReferenceIdString(testVar);
        }
        
        if (std::is_base_of_v<OsduMetadataStructBase, T>) {
            return testVar.isValid();
        }

        return false;
    }

private:
    std::string osdu_partition_;

    static inline const std::string osdu_static_{"osdu"};
    static inline const std::string wks_static_{"wks"};

    static inline const std::string dummyNamespace{"namespace"};
    static inline const std::string separator_{":"};
    static inline const std::string dot_{"."};
};

} // namespace segysdk::metadata::osdu
