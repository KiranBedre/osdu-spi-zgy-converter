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

#include <algorithm>
#include <json/json.h>
#include <optional>
#include <string>
#include <vector>

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduMetadataStructBase
{
    explicit OsduMetadataStructBase(std::string osduPartition);
    OsduMetadataStructBase(const OsduMetadataStructBase& other) = default;
    OsduMetadataStructBase(OsduMetadataStructBase&& other) noexcept;
    OsduMetadataStructBase& operator=(const OsduMetadataStructBase& rhs) = default;
    OsduMetadataStructBase& operator=(OsduMetadataStructBase&& rhs) noexcept;
    virtual ~OsduMetadataStructBase() = default;

    virtual bool isValid() const = 0;

    virtual std::string getPartition() const { return partition_; }
    virtual void propagatePartition(const std::string& partition);
    virtual void setPartition(std::string partition);

    virtual std::vector<std::string> performAttributeValidityScan() const = 0;
    virtual void appendStringVector(std::vector<std::string>& targetVector, const std::vector<std::string>& fromVector) const;
    virtual bool isValidOSDUCoordinateReferenceIdString(const std::string& attributeValue) const;
    virtual bool isValidOSDUReferenceIdString(const std::string& attributeValue) const;
    virtual bool isValidOSDUStorageSystemIdString(const std::string& idValue) const;
    virtual bool validPartitionCheck(const std::string& partition) const;

    static std::string getPartitionFromReference(const std::string& refAttributeString);

protected:
    virtual std::string getValidReferenceString(const std::string& partition, const std::string& areaPath, const std::string& className, const std::string& version = "") const;

    static bool isValidStringRegex(const std::string& str, const std::string& regex);
    bool isValidStringRegex(const std::optional<std::string>& str, const std::string& regex) const;

    bool isValidStringArrayRegex(const std::vector<std::string>& strs, const std::string& regex) const;
    bool isValidStringArrayRegex(const std::optional<std::vector<std::string>>& strs, const std::string& regex) const;

    enum class IdStringCheckType {
        CRS,
        Reference,
        StorageSystem
    };
    bool isValidIdString(const std::string& id, IdStringCheckType checkType) const;
    bool isValidIdString(const std::optional<std::string>& id, IdStringCheckType checkType) const;

    bool isValidIdStringArray(const std::vector<std::string>& ids, IdStringCheckType checkType) const;
    bool isValidIdStringArray(const std::optional<std::vector<std::string>>& ids, IdStringCheckType checkType) const;

    void performStringRegexValidityScan(std::vector<std::string>& results, const std::string& str, const std::string& tag, const std::string& regex) const;
    void performStringRegexValidityScan(std::vector<std::string>& results, const std::optional<std::string>& str, const std::string& tag, const std::string& regex) const;

    void performStringArrayRegexValidityScan(std::vector<std::string>& results, const std::vector<std::string>& strs, const std::string& tag, const std::string& regex) const;
    void performStringArrayRegexValidityScan(std::vector<std::string>& results, const std::optional<std::vector<std::string>>& strs, const std::string& tag, const std::string& regex) const;

    void performIdStringValidityScan(std::vector<std::string>& results, const std::string& id, const std::string& tag, IdStringCheckType checkType) const;
    void performIdStringValidityScan(std::vector<std::string>& results, const std::optional<std::string>& id, const std::string& tag, IdStringCheckType checkType) const;

    void performIdStringArrayValidityScan(std::vector<std::string>& results, const std::vector<std::string>& ids, const std::string& tag, IdStringCheckType checkType) const;
    void performIdStringArrayValidityScan(std::vector<std::string>& results, const std::optional<std::vector<std::string>>& ids, const std::string& tag, IdStringCheckType checkType) const;

    template<typename Struct>
    void performStructValidityScan(std::vector<std::string>& results, const Struct& structure) const
    {
        if (!structure.isValid()) {
            appendStringVector(results, structure.performAttributeValidityScan());
        }
    }
    template<typename Struct>
    void performStructValidityScan(std::vector<std::string>& results, const std::optional<Struct>& structure) const
    {
        if (structure.has_value()) {
            performStructValidityScan(results, structure.value());
        }
    }

    template<typename Struct>
    void performArrayStructValidityScan(std::vector<std::string>& results, const std::vector<Struct>& array) const
    {
        for (const auto& structure : array) {
            if (!structure.isValid()) {
                appendStringVector(results, structure.performAttributeValidityScan());
            }
        }
    }
    template<typename Struct>
    void performArrayStructValidityScan(std::vector<std::string>& results, const std::optional<std::vector<Struct>>& array) const
    {
        if (array.has_value()) {
            performArrayStructValidityScan(results, array.value());
        }
    }

    template<typename Struct>
    static bool structValid(const Struct& structure)
    {
        return structure.isValid();
    }
    template<typename Struct>
    bool structValid(const std::optional<Struct>& structure) const
    {
        return !structure.has_value() or structValid(structure.value());
    }

    template<typename Struct>
    bool arrayStructValid(const std::vector<Struct>& array) const
    {
        return std::all_of(array.begin(), array.end(), [](const Struct& structure) { return structure.isValid(); });
    }
    template<typename Struct>
    bool arrayStructValid(const std::optional<std::vector<Struct>>& array) const
    {
        return !array.has_value() or arrayStructValid(array.value());
    }

    inline static constexpr const char* const refSeparator_ = ":";

    inline static constexpr const char* const referenceStringRegex_ = "^[\\w.-]+:[\\w.-]+:[\\w.:\\\\%-]+:[0-9]*$";
    inline static constexpr const char* const contentSchemaKindStringRegex_ = "^[\\w.-]+:[\\w.-]+:[\\w.-]+:[0-9]+.[0-9]+.[0-9]+$";

    inline static constexpr const char* const numericStringRegex_ = "^[0-9]+$";
    inline static constexpr const char* const hexEvenNumCharStringRegex_ = "^([0-9a-fA-F]{2})+$";
    // inline static constexpr const char* const emailStringRegex_ = "^[a-zA-Z0-9_+&-]+(?:.[a-zA-Z0-9_+&-]+)*@(?:[a-zA-Z0-9-]+.)+[a-zA-Z]{2,7}$";
    // The above regexp doesn't handle email of the form "data.default.owners@opendes.enterprisedata.cloud.slb-ds.com"
    inline static constexpr const char* const emailStringRegex_ = "^.*@.*\\..*$";
    inline static constexpr const char* const countryCodeStringRegex_ = "^[A-Z]{2}$";
    inline static constexpr const char* const complianceStringRegex_ = "^(compliant|uncompliant)$";

private:
    static std::vector<std::string> parseReferenceAttribute(const std::string& osduReference);

private:
    std::string partition_;

};

} // namespace segysdk::metadata::osdu
