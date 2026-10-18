/*
Copyright 2022 Schlumberger

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

/** @file
 * @brief SegyIndexFactory.h contains declaration of the SegyIndexFactory, which is used to create an instance of the SegyIndex class.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReaderOptions.h"
#include "../SegyIO/SegyFile.h"
#include "../SegyIndex/SegyIndex.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace segysdk::api
{

/**
 * @class SegyIndexFactory
 *
 * @brief Used to create an instance of the SegyIndex class. This is the main interface to the SEG-Y SDK functionality.
 *
 **/
class SEGYSDK_DLL_PUBLIC SegyIndexFactory
{
public:
    /**
     * @brief Creates SegyIndexFactory struct instance
     **/
    SegyIndexFactory();

    /**
     *  @brief CreateSegyIndex  Creates and  returns a new SegyIndex instance.
     *
     * @param segyFilesVector gives the SEGY file(s) to build the trace information by
     * @param segyFileNames  vector of SEGY file path name(s) to operate with the index for.
     * @param fileReaderOptions  Defines attributes to get access to the file
     * @param indexBaseFileNameOverride  file path name to be used instead of the default one (the first file in the segyFileNames parameter)
     **/
    std::shared_ptr<segyindex::SegyIndex>
    CreateSegyIndex(std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
                    std::vector<std::string> const& segyFileNames,
                    segysdk::fileio::FileReaderOptions const& fileReaderOptions,
                    std::optional<std::string> const& indexBaseFileNameOverride = std::nullopt) const;

    std::shared_ptr<segyindex::SegyIndex> CreateSegyIndex(const segyindex::Params& params) const;
};

} // namespace segysdk::api
