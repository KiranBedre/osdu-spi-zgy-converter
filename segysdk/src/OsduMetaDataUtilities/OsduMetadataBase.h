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

#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"
#include "../segysdk_details/DLL_EXPORT.h"
#include "../Logger/Logger.h"

#include <optional>
#include <memory>
#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduMetadataBase
{
public:
    explicit OsduMetadataBase(std::string osduPartition, bool isArrayElement = false);
    explicit OsduMetadataBase(Json::Value json, std::string osduPartition, bool isArrayElement = false);
    OsduMetadataBase(const OsduMetadataBase& other) = default;
    OsduMetadataBase(OsduMetadataBase&& other) noexcept;
    OsduMetadataBase& operator=(const OsduMetadataBase& rhs) = default;
    OsduMetadataBase& operator=(OsduMetadataBase&& rhs) noexcept;
    virtual ~OsduMetadataBase();

public:
    /// \brief Assign an existing json object as the main root element
    /// \param json  Input json object
    void assignRootJsonObject(Json::Value json);

    /// \brief remove a member parameter in the json object (if it exists ... )
    void removeMember(const std::string& jsonPath);

    /// \brief Copy the members of the Json to be merged into the current myJson_ at root level, rather than add it as a separate node.
    /// \details Intended to be used e.g. for handling optional abstract modules, such as AbstractCommonResources.
    /// \param json  json-node to merge in
    virtual void mergeInJson(const Json::Value& json);

    /// \brief Recursively merge Json object into current myJson_ starting from root level.
    /// \details Intended to be used e.g. for populating different structures in data top-level element
    /// \param json  json-node to merge in
    virtual void mergeInJsonRecursively(const Json::Value& json, bool overwriteConflicting = false);

    //-------------

    /// \brief Get type T element from a Json object
    /// \param jsonPath    Path to the Json element to get
    template<typename T>
    T getParameter(const std::string& jsonPath) const
    {
        auto jsonPtr = getJsonElement(jsonPath);
        if (jsonPtr) {
            if constexpr (std::is_same_v<T, std::string>) {
                return jsonPtr->asString();
            }
            else if constexpr (std::is_same_v<T, int>) {
                return jsonPtr->asInt();
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return jsonPtr->asBool();
            }
            else if constexpr (std::is_same_v<T, double>) {
                return jsonPtr->asDouble();
            }
            else if constexpr (std::is_same_v<T, Json::Value>) {
                return *jsonPtr;
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                return jsonPtr->asInt64();
            }
            else if constexpr (std::is_same_v<T, uint64_t>) {
                return jsonPtr->asUInt64();
            }
            else if constexpr (std::is_same_v<T, size_t>) {
                return jsonPtr->asLargestUInt();
            }
            else {
                SEGYLOG_REPORT_WARNING("osduMetadata; getParameter: Specified type for " + jsonPath + " was not recognized");
                return {};
            }
        }
        return {};
    }

    /// \brief Get optional type T element from a Json object
    /// \param jsonPath    Path to the Json element to get
    template<typename T>
    std::optional<T> getOptionalParameter(const std::string& jsonPath) const
    {
        auto jsonPtr = getJsonElement(jsonPath);
        try {
            if (jsonPtr) {
                if constexpr (std::is_same_v<T, std::string>) {
                    return std::string(jsonPtr->asString());
                }
                else if constexpr (std::is_same_v<T, int>) {
                    return jsonPtr->asInt();
                }
                else if constexpr (std::is_same_v<T, bool>) {
                    return jsonPtr->asBool();
                }
                else if constexpr (std::is_same_v<T, double>) {
                    return jsonPtr->asDouble();
                }
                else if constexpr (std::is_same_v<T, Json::Value>) {
                    return *jsonPtr;
                }
                else if constexpr (std::is_same_v<T, int64_t>) {
                    return static_cast<int64_t>(jsonPtr->asInt64());
                }
                else if constexpr (std::is_same_v<T, uint64_t>) {
                    return static_cast<uint64_t>(jsonPtr->asUInt64());
                }
                else if constexpr (std::is_same_v<T, size_t>) {
                    return static_cast<size_t>(jsonPtr->asLargestUInt());
                }
                else {
                    SEGYLOG_REPORT_WARNING("osduMetadata; getOptionalParameter: Specified type for " + jsonPath + " was not recognized");
                    return std::nullopt;
                }
            }
            return std::nullopt;
        }
        catch (const std::exception& ex) {
            SEGYLOG_REPORT_WARNING("osduMetadata; getOptionalParameter: Specified type for " + jsonPath + " was not recognized. Exception :" + std::string(ex.what()));        
        }
        return std::nullopt;
    }

    /// \brief Get array of type T elements from a Json object
    /// \param jsonPath    Path to the Json element to get
    template<typename T>
    std::vector<T> getArrayParameter(const std::string& jsonPath) const
    {
        return getAllArrayElements<T>(jsonPath);
    }

    /// \brief Get array of type T elements from a Json object
    /// \param jsonPath    Path to the Json element to get
    template<typename T>
    std::optional<std::vector<T>> getOptionalArrayParameter(const std::string& jsonPath) const
    {
        auto result = getAllArrayElements<T>(jsonPath);
        return !result.empty() ? std::optional(result) : std::nullopt;
    }

    /// \brief Get structure of type Struct from a Json object using metadata class T
    /// \param jsonPath    Path to the Json element to get structure from or an empty string to use root element
    /// \return            Filled in structure or an empty one with initialized partition in case of error
    template<typename T, typename Struct>
    Struct getStructParameter(const std::string& jsonPath = "") const
    {
        try {
        
        auto structure = getOptionalStructParameter<T, Struct>(jsonPath);
        return structure.has_value() ? structure.value() : Struct{getOsduPartition()};
        } 
        catch (const std::exception& ex) {
            SEGYLOG_REPORT_ERROR("osduMetadata; getStructParameter:" + std::string(ex.what()));
        }

        return Struct{getOsduPartition()};
    }

    /// \brief Get optional structure of type Struct from a Json object using metadata class T
    /// \param jsonPath    Path to Json element to get structure from or an empty string to use root element
    /// \return            std::optional with filled in structure or std::nullopt in case of error
    template<typename T, typename Struct>
    std::optional<Struct> getOptionalStructParameter(const std::string& jsonPath = "") const
    {
        try {
            auto jsonPtr = getJsonElement(jsonPath);
            if (jsonPtr) {
                T object(*jsonPtr, getOsduPartition());
                auto structurePtr = std::reinterpret_pointer_cast<Struct>(object.getMetaDataStruct());
                if (structurePtr != nullptr) {
                    return {*structurePtr};
                }
            }
        }
        catch (const std::exception& ex) {
            SEGYLOG_REPORT_ERROR("osduMetadata; getOptionalStructParameter:" + std::string(ex.what()));
        }
        return std::nullopt;
    }

    /// \brief Get array of structures of type Struct from a Json element using metadata class T
    /// \param jsonPath    Path to Json element to get an array of structures from or an empty string to use root element
    /// \return            Array of structure objects or an empty array in case of error
    template<typename T, typename Struct>
    std::vector<Struct> getArrayStructParameter(const std::string& jsonPath = "") const
    {
        auto array = getOptionalArrayStructParameter<T, Struct>(jsonPath);
        return array.has_value() ? array.value() : std::vector<Struct>{};
    }

    /// \brief Get optional array of structures of type Struct from a Json element using metadata class T
    /// \param parameterName    Path to the Json element to get an array of structures from or an empty string to use root element
    /// \return                 Array of structure objects or std::nullopt if no elements available
    template<typename T, typename Struct>
    std::optional<std::vector<Struct>> getOptionalArrayStructParameter(const std::string& jsonPath = "") const
    {
        std::vector<Struct> array;
        const auto jsonElements = getAllArrayElements<Json::Value>(jsonPath);
        for (const auto& jsonElement : jsonElements) {
            T object(jsonElement, getOsduPartition());
            const auto structPtr = std::reinterpret_pointer_cast<Struct>(object.getMetaDataStruct());
            if (structPtr != nullptr) {
                array.emplace_back(*structPtr);
            }
        }

        return array.empty() ? std::nullopt : std::optional(array);
    }

    /// \brief Set type T element in a Json object
    /// \param jsonPath    Name of the Json element to update
    /// \param value            new value of the specified element
    template<typename T>
    void setParameter(const std::string& jsonPath, T value)
    {
        auto jsonPtr = getJsonElement(jsonPath);
        if (jsonPtr) {
            if constexpr (std::is_same_v<T, std::string>
                          or std::is_same_v<T, int>
                          or std::is_same_v<T, bool>
                          or std::is_same_v<T, double>
                          or std::is_same_v<T, Json::Value>) {
                *jsonPtr = value;
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                *jsonPtr = static_cast<Json::Int64>(value);
            }
            else if constexpr (std::is_same_v<T, uint64_t>) {
                *jsonPtr = static_cast<Json::UInt64>(value);
            }
            else if constexpr (std::is_same_v<T, size_t>) {
                *jsonPtr = static_cast<Json::LargestUInt>(value);
            }
            else {
                SEGYLOG_REPORT_ERROR("osduMetadata; setParameter: Specified type for " + jsonPath + " was not recognized");
            }
        }
        else {
            SEGYLOG_REPORT_ERROR("osduMetadata; setParameter: Failed to create " + jsonPath + " element");
        }
    }

    /// \brief Set optional type T element in a Json object
    /// \param parameterName    Name of the Json element to update
    /// \param value            new value of the specified element

    template<typename T>
    void setOptionalParameter(const std::string& parameterName, T value)
    {
        {
            if (value.has_value()) {
                setParameter(parameterName, value.value());
            }
            else {
                removeMember(parameterName);
            }
        }
    }

    /// \brief Add array of type T elements to a Json object
    /// \param parameterName    Name of the Json element to update
    /// \param array            Values to add
    template<typename T>
    void setArrayParameter(const std::string& parameterName, const std::vector<T>& array)
    {
        for (const auto& value : array) {
            appendArrayElement(parameterName, value);
        }
    }

    /// \brief Add optional array of type T elements to a Json object
    /// \param parameterName    Name of the Json element to update
    /// \param array            Values to add
    template<typename T>
    void setOptionalArrayParameter(const std::string& parameterName, const std::optional<std::vector<T>>& array)
    {
        if (array.has_value()) {
            setArrayParameter(parameterName, array.value());
        }
    }

    //-------------
    /// \brief Get all type T elements from a Json array element
    /// \param jsonPath    Path to the Json element to query
    /// \return            vector holding all type T elements for requested parameter name
    template<typename T>
    std::vector<T> getAllArrayElements(const std::string& jsonPath) const
    {
        std::vector<T> result;

        auto jsonArrayPtr = getJsonElement(jsonPath);
        if (jsonArrayPtr and jsonArrayPtr->isArray()) {
            for (const auto& json : *jsonArrayPtr) {
                if constexpr (std::is_same_v<T, std::string>) {
                    result.emplace_back(json.asString());
                }
                else if constexpr (std::is_same_v<T, int>) {
                    result.emplace_back(json.asInt());
                }
                else if constexpr (std::is_same_v<T, bool>) {
                    result.emplace_back(json.asBool());
                }
                else if constexpr (std::is_same_v<T, double>) {
                    result.emplace_back(json.asDouble());
                }
                else if constexpr (std::is_same_v<T, int64_t>) {
                    result.emplace_back(json.asInt64());
                }
                else if constexpr (std::is_same_v<T, uint64_t>) {
                    result.emplace_back(json.asUInt64());
                }
                else if constexpr (std::is_same_v<T, size_t>) {
                    result.emplace_back(json.asLargestUInt());
                }
                else if constexpr (std::is_same_v<T, Json::Value>) {
                    result.emplace_back(json);
                }
                else {
                    SEGYLOG_WARNING("getAllArrayElements: Specified type for " + jsonPath + " was not recognized");
                    continue;
                }
            }
        }
        return result;
    }

    //-------------
    /// \brief Get type T element from a Json array
    /// \param jsonPath         Path to the Json element to query
    /// \param pos              position of requested array element
    /// \return                 vector holding all type T elements for requested parameter name
    template<typename T>
    T getArrayElement(const std::string& jsonPath, int pos) const
    {
        auto jsonArrayPtr = getJsonElement(jsonPath);
        if (jsonArrayPtr and jsonArrayPtr->isArray() and jsonArrayPtr->isValidIndex(pos)) {
            auto json = (*jsonArrayPtr)[pos];
            if constexpr (std::is_same_v<T, std::string>) {
                return json.asString();
            }
            else if constexpr (std::is_same_v<T, int>) {
                return json.asInt();
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return json.asBool();
            }
            else if constexpr (std::is_same_v<T, double>) {
                return json.asDouble();
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                return json.asInt64();
            }
            else if constexpr (std::is_same_v<T, uint64_t>) {
                return json.asUInt64();
            }
            else if constexpr (std::is_same_v<T, size_t>) {
                return json.asLargestUInt();
            }
            else if constexpr (std::is_same_v<T, Json::Value>) {
                return json;
            }
            else {
                SEGYLOG_REPORT_ERROR("getArrayElement: Specified type for " + jsonPath + " was not recognized");
            }
        }
        return {};
    }


    /// \brief Append a type T element to a Json array element
    /// \param jsonPath         Path to the Json element to query
    /// \param value            new array element to be added
    template<typename T>
    void appendArrayElement(const std::string& jsonPath, T value)
    {
        auto jsonArrayPtr = getJsonElement(jsonPath);
        if (!jsonArrayPtr) {
            SEGYLOG_WARNING("appendArrayElement: Failed to create " + jsonPath + " element");
        }

        if (*jsonArrayPtr == Json::nullValue) { // Newly created element
            *jsonArrayPtr = Json::arrayValue;
        }

        if (jsonArrayPtr->isArray()) {
            if constexpr (std::is_same_v<T, std::string>
                          or std::is_same_v<T, int>
                          or std::is_same_v<T, bool>
                          or std::is_same_v<T, double>
                          or std::is_same_v<T, Json::Value>) {
                jsonArrayPtr->append(value);
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                jsonArrayPtr->append(static_cast<Json::Int64>(value));
            }
            else if constexpr (std::is_same_v<T, uint64_t>) {
                jsonArrayPtr->append(static_cast<Json::UInt64>(value));
            }
            else if constexpr (std::is_same_v<T, size_t>) {
                jsonArrayPtr->append(static_cast<Json::LargestUInt>(value));
            }
            else {
                SEGYLOG_REPORT_WARNING("osduMetadata; appendArrayElement: Specified type to append to " + jsonPath + " was not recognized");
                jsonArrayPtr->append(value);
            }
        }
    }

    template<typename T, typename Struct>
    void setStructParameter(const std::string& parameterName, const Struct& structure)
    {
        setParameter(parameterName, T(structure).getAsJson());
    }

    template<typename T, typename Struct>
    void setOptionalStructParameter(const std::string& parameterName, const std::optional<Struct>& structure)
    {
        if (structure.has_value()) {
            setStructParameter<T>(parameterName, structure.value());
        }
    }

    template<typename T, typename Struct>
    void setArrayStructParameter(const std::string& parameterName, const std::vector<Struct>& array)
    {
        for (const auto& structure : array) {
            appendArrayElement(parameterName, T(structure).getAsJson());
        }
    }

    template<typename T, typename Struct>
    void setOptionalArrayStructParameter(const std::string& parameterName, const std::optional<std::vector<Struct>>& array)
    {
        if (array.has_value()) {
            setArrayStructParameter<T>(parameterName, array.value());
        }
    }

    /// \brief Append a type T element to a Json array element
    /// \param value            new array element to be added
    template<typename T>
    void appendRootArrayElement(T value)
    {
        if (isRootJsonArray()) {
            if constexpr (std::is_same_v<T, std::string>) {
                myJson_.append(value);
            }
            else if constexpr (std::is_same_v<T, int>) {
                myJson_.append(value);
            }
            else if constexpr (std::is_same_v<T, bool>) {
                myJson_.append(value);
            }
            else if constexpr (std::is_same_v<T, double>) {
                myJson_.append(value);
            }
            else if constexpr (std::is_same_v<T, int64_t>) {
                myJson_.append(static_cast<Json::Int64>(value));
            }
            else if constexpr (std::is_same_v<T, uint64_t>) {
                myJson_.append(static_cast<Json::UInt64>(value));
            }
            else if constexpr (std::is_same_v<T, size_t>) {
                myJson_.append(static_cast<Json::LargestUInt>(value));
            }
            else if constexpr (std::is_same_v<T, Json::Value>) {
                myJson_.append(value);
            }
            else {
                SEGYLOG_WARNING("addRootArrayElement: Specified type to append was not recognized");
                myJson_.append(value);
            }
        }
    }

    //-------------

   ///\brief Get all type T elements from a Json root array element
   /// \return                 vector holding all type T elements from the root json object
    template<typename T>
    std::vector<T> getAllRootArrayElements() const
    {
        std::vector<T> result;

        if (myJson_.isArray()) {
            for (const auto& element : myJson_) {
                if constexpr (std::is_same_v<T, std::string>) {
                    result.emplace_back(element.asString());
                }
                else if constexpr (std::is_same_v<T, int>) {
                    result.emplace_back(element.asInt());
                }
                else if constexpr (std::is_same_v<T, bool>) {
                    result.emplace_back(element.asBool());
                }
                else if constexpr (std::is_same_v<T, double>) {
                    result.emplace_back(element.asDouble());
                }
                else if constexpr (std::is_same_v<T, int64_t>) {
                    result.emplace_back(element.asInt64());
                }
                else if constexpr (std::is_same_v<T, uint64_t>) {
                    result.emplace_back(element.asUInt64());
                }
                else if constexpr (std::is_same_v<T, size_t>) {
                    result.emplace_back(element.asLargestUInt());
                }
                else if constexpr (std::is_same_v<T, Json::Value>) {
                    result.emplace_back(element);
                }
                else {
                    SEGYLOG_REPORT_WARNING("osduMetadata; getAllRootArrayElements: Specified jsonPtr type was not recognized");
                    result.emplace_back(element);
                }
            }
        }
        return result;
    }

    //-------------
    /// \brief Initialize the json root object to be an empty array
    void InitializeJsonRootAsArray();


    /// \brief Initialize an array element in the json object to an empty array
    /// \param parameterName  Name of node element to initialize
    virtual void InitializeJsonArray(const std::string& jsonPath);

    /// \brief Get element count of a Json array element
    /// \param parameterName    Name of the Json element to check
    /// \return                 int holding number of elements. 0 or positive for an actual element, -1 if the field is not an array (or if the field is not a member of the json object)
    virtual int64_t getArraySize(const std::string& jsonPath) const;

    /// \brief Is current field a Json array element?
    /// \param parameterName    Name of the Json element to check
    /// \return                 true if element is an array, false if the field is not an array (or if the field is not a member of the json object)
    virtual bool isJsonArray(const std::string& parameterName) const;

    /// \brief Is current root object a Json array?
    /// \return                 true if root is an array, false if the root object is not an array
    virtual bool isRootJsonArray() const;

    /// \brief Is current root object an empty Json array?
    /// \return                 true if root is an empty array, false if the root object is not an array or not empty
    virtual bool isRootEmptyArray() const;

    /// \brief Check if requested Array object is empty or not.
    /// \param parameterName Json element to check
    /// \return  true means array object is empty, false means it has members
    virtual bool isEmptyArray(const std::string& jsonPath) const;


    //-------------

    /// \brief string representation of the json object. This will be as a nicely formatted output string
    /// \return string holding the value of the current json object
    virtual std::string getAsFormattedString() const;

    /// \brief string representation of the json object. This will be as a compactly formatted output string to reduce number of bytes required to hold the string.
    /// \return string holding the value of the current json object
    virtual std::string getAsCompactString() const;

    /// \brief Returns a list of strings for the current Json object (only 1 level)
    /// \return list of strings with the defined members (or nodes) of the current json object
    virtual Json::Value getAsJson() const;

    /// \brief Returns the current Json object
    /// \return json object representing the current metadata object
    virtual std::vector<std::string> getJsonMemberNames() const;

    /// \brief This will validate that mandatory objects are defined in the input struct or parameters
    /// \return  true indicates input data is OK, false otherwise
    virtual bool isValidInput() const = 0;

    /// \brief Refreshes metadata struct from Json object and return metadata struct
   /// \return  pointer to encapsulated metadata
    virtual std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() = 0;

    /// \brief resets the current Json object to a clean state
    virtual void clearJsonObject() { myJson_.clear(); }

    /// \brief checks if jsonPath is actually a member of the internal Json object
    /// \param jsonPath    Path to Json element to check existance
    /// \return
    virtual bool isMember(const std::string& jsonPath) const;

private:
    /// \brief Reset members of Json struct
    virtual void initializeJson() = 0;

    /// \brief Private member method to populate the json object from the provided parameters or data struct
    virtual void populateJsonObject() = 0;

    /// \brief Reset members of metadata struct
    virtual void initializeMetadataStruct() = 0;

    /// \brief Refreshes internal metadata struct from Json object
    virtual void populateMetadataStruct() = 0;

    /// \brief Perform sanity checks before attempting to retrieve a json array element
    /// \param parameterName   name of attribute to append to
    /// \param index           array element requested
    bool isValidArrayReference(const std::string& parameterName, Json::ArrayIndex index) const;

protected:
    /// \brief Method to get the current Json::Value root object
    /// \return Json::Value with current json object
    Json::Value getJson() const { return myJson_; }

    /// \brief Method to set the current Json::Value root object
    /// \param value new Json::Value object to use
    void setJson(Json::Value value) { myJson_ = std::move(value); }

    /// \brief Method to get the status flag indicating whether the current object is meant to be a regular json object or an array of elements
    /// \return true if the current object is an array, false if there will be members.
    bool getIsArrayElement() const { return isArrayElement_; }

    /// \brief Method to set the isArrayElement status flag
    /// \param isArray  bool value of true means this will be an array of elements, false means it will be a regular json object (this is the normal mode).
    void setIsArrayElement(bool isArray) { isArrayElement_ = isArray; }

    /// \brief Method to get the OSDU partition to use from the base class, deliberately with a different name than for StructBase
    /// \return  string with the current OSDU partition
    std::string getOsduPartition() const { return partition_; }

    /// \brief Method to set the OSDU partition in the base class
    /// \param  value  string with the OSDU partition
    void setOsduPartition(std::string value) { partition_ = std::move(value); }

private:
    /// \brief Recursively merge source Json object into target.
    /// \param target  json-node to merge into
    /// \param source  json-node to merge
    /// \param overwriteConflicting  in case of conflicing json leaf nodes, prefer values from source. Otherwise keep ones from target
    static void mergeJsonRecursively(Json::Value& target, const Json::Value& source, bool overwriteConflicting = false);

    static std::vector<std::string> parseJsonPath(const std::string& jsonPath);
    static std::string toJsonPath(const std::vector<std::string>& elementsChain);

    Json::Value* getJsonElement(const std::string& jsonPath);
    Json::Value* getJsonElement(const std::vector<std::string>& elementsChain);

    const Json::Value* getJsonElement(const std::string& jsonPath) const;
    const Json::Value* getJsonElement(const std::vector<std::string>& elementsChain) const;

private:
    Json::Value myJson_{};

    bool isArrayElement_;

    std::string partition_;
};

} // namespace segysdk::metadata::osdu
