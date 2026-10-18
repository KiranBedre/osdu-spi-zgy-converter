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

#include "../SegyFormat/External/SegyContext.h"
#include "../SegyIO/FileHeaderDescription.h"
#include "../SegyIO/HeaderDataType.h"
#include "../SegyIO/HeaderDefinition.h"
#include "../SegyIO/HeaderOverride.h"

#include <memory>

namespace segysdk::segyio
{

/**
* @class FileHeaderDescriptionFactory
*
* @brief Factory class for creating instances of the FileHeaderDescription.
*
**/
class SEGYSDK_DLL_PUBLIC FileHeaderDescriptionFactory
{
private:
    std::shared_ptr<segyformat::external::SegyContext> context_;
    void InitFileHeaderDescription() const;

    std::string getNativeKeyFromUserKey(const std::shared_ptr<std::map<std::string, std::string>>& traceHdrMap, const std::string& myKey) const;
    bool isWellKnownKey(const std::shared_ptr<std::map<std::string, std::string>>& traceHdrMap, const std::string& myKey) const;

    std::shared_ptr<std::map<std::string, std::string>> getBinaryHeaderMap() const;
    std::shared_ptr<std::map<std::string, std::string>> getTraceHeaderMap() const;
    segyio::HeaderDataType getDataType(const segyformat::external::DataFormat& fmt) const;
    void clearAnyOverlaps(std::vector<std::shared_ptr<HeaderDefinition>> headers, const std::shared_ptr<HeaderDefinition>& fld) const;
    std::shared_ptr<segysdk::segyio::HeaderDefinition> GetHeaderDefinition(
        const std::shared_ptr<segysdk::segyformat::external::SegyHeaderField>& hdr) const;

public:
    /**
        * *
        * @brief Default constructor for the FileHeaderDescriptionFactory.
        *
        **/
    explicit FileHeaderDescriptionFactory(std::shared_ptr<segyformat::external::SegyContext> context);

    /**
       * 
       * @brief Gets a new FileHeaderDescription instance.
       *
       * @returns a pointer to the FileHeaderDescription instance
       * 
       **/
    std::shared_ptr<FileHeaderDescription> GetFileHeaderDesctription() const;

    /**
        *
        * @brief Generates a vector of pointers to the full binary header definiiton used by the FileHeaderDescription.
        * 
        * @details The returned binary header definition is a merge between possibly sparse definition provided by the user, possibly only containing non standard field definitions, and the standard binary header definition according to the SEG-Y version of the input file(s)
        *
        * @returns a vector holding the binary header definiiton in the current FileHeaderDescription instance.
        *
        **/
    std::vector<std::shared_ptr<HeaderDefinition>> Generate_binaryHeaders() const;

    /**
       *
       * @brief Generates a vector of pointers to the full trace header definiiton used by the FileHeaderDescription.
       *
       * @details The returned trace header definition is a merge between possibly sparse definition provided by the user, possibly only containing non standard field definitions, and the standard binary header definition according to the SEG-Y version of the input file(s)
       *
       * @returns a vector holding the trace header definiiton in the current FileHeaderDescription instance.
       *
       **/
    std::vector<std::shared_ptr<HeaderDefinition>> Generate_traceHeaders() const;

    /**
      *
      * @brief Generates a vector of pointers to the binary header override definiiton used by the FileHeaderDescription.
      *
      * @returns a vector holding the binary header override definiiton in the current FileHeaderDescription instance.
      *
      **/
    const std::vector<std::shared_ptr<HeaderOverride>> GetBinaryHeaderOverrides() const;

    /**
      *
      * @brief Generates a vector of pointers to the trace header override definiiton used by the FileHeaderDescription.
      *
      * @returns a vector holding the trace header override definiiton in the current FileHeaderDescription instance.
      *
      **/
    const std::vector<std::shared_ptr<HeaderOverride>> GetTraceHeaderOverrides() const;
};

} // namespace segysdk::segyio
