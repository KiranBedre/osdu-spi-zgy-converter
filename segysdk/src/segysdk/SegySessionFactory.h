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

/** @file
 * @brief SegySessionFactory.h contains declaration of the SegySessionFactory, which is used to create an instance of the SegySession class. This is the main interface to the SEG-Y SDK functionality.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../SegyFormat/External/SegyContext.h"
#include "../segysdk/SegySession.h"

#include <memory>
#include <string>

namespace segysdk::api
{

/**
 * @class SegySessionFactory
 *
 * @brief Used to create an instance of the SegySession class. This is the main interface to the SEG-Y SDK functionality.
 *
 **/
class SEGYSDK_DLL_PUBLIC SegySessionFactory
{
public:
    /**
     * @brief Constructor - Standard constructor for DELFI located Seismic Data Management Services (SDMS) when defining a multi SEG-Y file dataset.
     *
     * @brief jsonParameters.
     * @details
     * This version of the SegySessionFactory can be used when running in the DELFI cloud environment.
     *
     *
     * @param segyFileNameList  vector<string> listing all SEG-Y files to be included.
     * @param jsonParameters    Json string specifying the loading parameter definition for the current SEG-Y files when running in the DELFI cloud environment.
     *
     * Example DELFI LoadingParameters structure below. Depending on how the input SEG-Y file was  generated, the trace header literals specified in the file below might be found in different byte locations than that specified by the SEG-Y standard. In this case the user must update the contents of the parameter file accordingly.
     * @snippet LoadingParameters.json DELFI_LoadingParameters example
     *
     **/
    SegySessionFactory(const std::vector<std::string>& segyFileNameList, std::string jsonParameters);

    /**
     * @brief Constructor - Standard constructor for OSDU Data Platform. This can be defining either a single or a multi SEG-Y file dataset.
     *
     * @details
     * This version of the SegySessionFactory uses the definitions in the osdu FileCollection.SEGY.1.0.0.  The fileCollection contains all required input, such as VectorHeaderMapping (byte location definition), and FileSourceInfos. (list of input file(s))
     *
     * @param osduFileCollectionSEGY    Json string representation of the OSDU FileCollection.SEGY.1.0.0 structure.
     *
     * Example OSDU FileCollection.SEGY.1.0.0 structure below. Depending on how the input SEG-Y file was  generated, the trace header literals specified in the file below might be found in different byte locations than that specified by the SEG-Y standard. In this case the user must update the contents of the parameter file VectorHeaderMapping section accordingly.
     * @snippet FileCollection.SEGY.json OSDU_FileCollection example
     *
     **/
    explicit SegySessionFactory(std::string osduFileCollectionSEGY);

    /**
     *  @brief getFileName  Helper function to return the SEG-Y file used by the factory.
     *
     * @param index         When running against SDMS in the cloud e.g. DELFI or OSDU this sets the login credentials to use when accessing the data. Default is SDCredentials::none which indicates use with local data files.
     * @return              std::string with the requested file name. If index is out of range, and empty string will be returned.
     **/
    std::string getFileName(size_t index = 0) const;

    /**
     *  @brief CreateSegySession  Creates and  returns a new SegySession instance.
     *
     * @param sdCredentials  When running against SDMS in the cloud e.g. DELFI or OSDU this sets the login credentials to use when accessing the data. Default is SDCredentials::none which indicates use with local data files.
     * @param cacheSize      Defines what cache size to use for any buffered read operations.
     **/
    std::unique_ptr<SegySession> CreateSegySession(const SDCredentials& sdCredentials = SDCredentials::none(), const CacheSizeOption& cacheSize = CacheSizeOption::none()) const;

    std::unique_ptr<SegySession> CreateSegySession(const segyio::StorageAccessParams& params) const;

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk::api
