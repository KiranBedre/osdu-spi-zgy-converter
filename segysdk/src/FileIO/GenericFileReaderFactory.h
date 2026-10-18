/*
Copyright 2021-2022 Schlumberger

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

#include "../FileIO/GenericFileReader.h"
#include "../Utilities/auth/TokenSource.h"

#include <memory>
#include <string>

namespace segysdk::api
{

/**
 * @class GenericFileReaderFactory
 *
 * @brief Used to create an instance of the SegySession class. This is the main interface to the SEG-Y SDK functionality.
 *
 **/
class SEGYSDK_DLL_PUBLIC GenericFileReaderFactory
{
public:
    /**
     * @brief Constructor - Constructor for the GenericFileReaderFactory.
     *
     * @details
     * This version of the GenericFileReaderFactory can be used when accessing local files.
     *
     * @param fileName  std::string full path to requested input file
     *
     **/
    explicit GenericFileReaderFactory(const std::string& fileName);

    /**
     * @brief Constructor - Constructor for the GenericFileReaderFactory.
     *
     * @details
    * This version of the GenericFileReaderFactory should be used when accessing files files hosted in cloud SDMS storage.
     *
     * @param fileName    std::string full path to requested input file
     * @param sdms_url    std::string URL to SDMS cloud service
     * @param api_key     std::string API key for cloud SDMS service 
     * @param tokenSource user token for accessing SDMS service being fetched via TokenSource interface
     *     
     **/
    GenericFileReaderFactory(const std::string& fileName,
                             const std::string& sdms_url,
                             const std::string& api_key,
                             std::unique_ptr<utilities::auth::TokenSource> tokenSource);

    /**
     *  @brief GenericFileReader  Creates and  returns a new GenericFileReader instance.
     *

     **/
    std::shared_ptr<fileio::GenericFileReader> CreateGenericFileReader() const;

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk::api
