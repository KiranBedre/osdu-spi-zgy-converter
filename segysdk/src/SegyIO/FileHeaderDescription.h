/*
Copyright 2021 Schlumberger

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

#include "../SegyIO/HeaderDefinition.h"
#include "../FileIO/FileReaderOptions.h"
#include "../FileIO/FileReference.h"
#include "../SegyIO/VerticalDomain.h"
#include "../SegyIO/VerticalUOM.h"

#include <json/json.h>

#include <map>
#include <memory>
#include <vector>

namespace segysdk::segyio
{

/**
  * @class FileHeaderDescription
  *
  * @brief Used to hold the internal representation of binary header and trace header definitions and possible override values for these.
  *
  * @details No public constructor is available for this class, it should be created using the FileHeaderDefinitionFactory
  *
  **/
class SEGYSDK_DLL_PUBLIC FileHeaderDescription
{
private:
    std::shared_ptr<std::vector<std::shared_ptr<HeaderDefinition>>> binaryHeaders_;
    std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> binaryHeaderOverrides_;
    std::shared_ptr<std::vector<std::shared_ptr<HeaderDefinition>>> traceHeaders_;
    std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> traceHeaderOverrides_;

private:
    std::shared_ptr<HeaderDefinition> getTraceHeaderDefinition(const std::string& headerId) const;

private:
    static ScalarUsageStrategy getScalarUsageStrategyFromString(const std::string& strategy);
    static HeaderDataType getHeaderDataTypeFromString(const std::string& dataType);
    static std::vector<std::shared_ptr<HeaderDefinition>> getHeaderDefinitionsFromJsonParameter(Json::Value node);
    static std::vector<std::shared_ptr<HeaderOverride>> getHeaderOverridesDefinitionFromJsonParameter(Json::Value node);

public:
    FileHeaderDescription(const std::vector<std::shared_ptr<HeaderDefinition>>& binaryHeaders, const std::vector<std::shared_ptr<HeaderDefinition>>& traceHeaders);

    ~FileHeaderDescription();

    FileHeaderDescription(const FileHeaderDescription& other);
    FileHeaderDescription& operator=(const FileHeaderDescription& other);

    FileHeaderDescription(FileHeaderDescription&& other) noexcept;
    FileHeaderDescription& operator=(FileHeaderDescription&& other) noexcept;

    /**
    *
    * @brief Generates a vector of pointers to the full binary header definition used by the FileHeaderDescription.
    *
    * @details The returned binary header definition is a merge between possibly sparse definition provided by the user, possibly only containing non standard field definitions, and the standard binary header definition according to the SEG-Y version of the input file(s)
    *
    * @returns std::vector<std::shared_ptr<HeaderDefinition>> holding the binary header definition in the current FileHeaderDescription instance.
    *
    **/
    std::shared_ptr<std::vector<std::shared_ptr<HeaderDefinition>>> getBinaryHeaders() const;

    /**
    *
    * @brief Generates a vector of pointers to the full trace header definition used by the FileHeaderDescription.
    *
    * @details The returned trace header definition is a merge between possibly sparse definition provided by the user, possibly only containing non standard field definitions, and the standard binary header definition according to the SEG-Y version of the input file(s)
    *
    * @returns std::vector<std::shared_ptr<HeaderDefinition>> holding the trace header definition in the current FileHeaderDescription instance.
    *
    **/
    std::shared_ptr<std::vector<std::shared_ptr<HeaderDefinition>>> getTraceHeaders() const;

    /**
    *
    * @brief Generates a vector of pointers to the binary header override definition used by the FileHeaderDescription.
    *
    * @returns std::vector<std::shared_ptr<HeaderOverride>> holding the binary header override definition in the current FileHeaderDescription instance.
    *
    **/
    std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> getBinaryHeaderOverrides() const;

    /**
    *
    * @brief Generates a vector of pointers to the trace header override definition used by the FileHeaderDescription.
    *
    * @returns std::vector<std::shared_ptr<HeaderOverride>> holding the trace header override definition in the current FileHeaderDescription instance.
    *
    **/
    std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> getTraceHeaderOverrides() const;

    /**
    *
    * @brief Sets the binary header override definition to be used by the FileHeaderDescription.
    *
    * @param  binaryHeaders Pointer to the vector holding the binary header override definition to be used by the FileHeaderDescription instance.
    *
    **/
    void setBinaryHeaderOverrides(const std::vector<const HeaderOverride*>& binaryHeaders) const;

    /**
   *
   * @brief Sets the trace header override definition to be used by the FileHeaderDescription.
   *
   * @param  traceHeaders Pointer to the vector holding the trace header override definition to be used by the FileHeaderDescription instance.
   *
   **/
    void setTraceHeaderOverrides(const std::vector<const HeaderOverride*>& traceHeaders) const;

    /**
     *
     * @brief Sets the binary header override definition to be used by the FileHeaderDescription.
     *
     * @param  binaryHeaders Pointer to the vector holding the binary header override definition to be used by the FileHeaderDescription instance.
     *
     **/
    void setBinaryHeaderOverrides(const std::vector<std::shared_ptr<HeaderOverride>>& binaryHeaders) const;

    /**
    *
    * @brief Sets the trace header override definition to be used by the FileHeaderDescription.
    *
    * @param  traceHeaders Pointer to the vector holding the trace header override definition to be used by the FileHeaderDescription instance.
    *
    **/
    void setTraceHeaderOverrides(const std::vector<std::shared_ptr<HeaderOverride>>& traceHeaders) const;

    /**
    *
    * @brief extracts FileHeaderDefinition from stored parameter file.
    *
    * @param  const std::string segyFileName is name of SEG-Y file to access.
    * @param  fileio::FileReaderOptions options is file options (connection parameters and cache info) for file.
    *
    * @returns FileHeaderDescription extracted from parameter file.
    **/
    static std::shared_ptr<FileHeaderDescription> loadFromParameterFile(const std::string& segyFilename, fileio::FileReaderOptions options);

    /**
    *
    * @brief extracts FileHeaderDefinition from stored parameter file.
    *
    * @param  const fileio::FileReference fileRef is a FileReference instantiated with the file name and reader options
    *
    * @returns FileHeaderDescription extracted from parameter file.
    **/
    static std::shared_ptr<FileHeaderDescription> loadFromParameterFile(const fileio::FileReference& fileRef);

    /**
    *
    * @brief Gets the vertical domain of the current SEG-Y file.
    *
    * @returns VerticalDomain enumerable inicating either Time or Depth.
    **/
    VerticalDomain getVerticalDomain() const;

    /**
     *
     * @brief Gets the vertical unit of measurements for current SEG-Y file.
     *
     * @returns VerticalUOM enumerable indicating the relevant unit of measure. (seconds, meter or feet)
     **/
    VerticalUOM getVerticalUOM() const;

    /**
     *
     * @brief Produces a json string with the definition of the current FileHeaderDefinition instance.
     *
     * @returns a json string with the definition of the current FileHeaderDefinition instance.
     **/
    std::string toJsonString() const;
    static Json::Value getSEGYHeaderJson(const std::vector<std::shared_ptr<HeaderDefinition>>& headers);
    static Json::Value getSEGYHeaderOverridesJson(const std::map<std::string, std::shared_ptr<HeaderOverride>>& overrides);

    /**
     *
     * @brief Produces a string with the definition of the requested HeaderDataType. Typically used when producing json dump of the header definitions.
     *
     * @returns string with the textual representation of the HeaderDataType.
     **/
    static std::string getHeaderDataTypeJson(const HeaderDataType dataType);

    /**
     *
     * @brief Produces a string with the definition of the requested ScalarUsageStrategy. Typically used when producing json dump of the header definitions.
     *
     * @returns string with the textual representation of the ScalarUsageStrategy.
     **/
    static std::string getHeaderScalarUsageStrategy(const ScalarUsageStrategy strategy);

    /**
     *
     * @brief Produces a string with the name of the companion parameter file (Not relevant in OSDU environment).
     *
     * @returns string with the fully qualified file name.
     **/
    static std::string getParameterFileName(const std::string& segyFileName);

    /**
     *
     * @brief Checks if the current SEG-Y file has a companion parameter file (Not relevant in OSDU environment).
     *
     * @returns bool true if parameter file exists.
     **/
    static bool parameterFileExists(const fileio::FileReference& segyFileReference);
};

} // namespace segysdk::segyio
