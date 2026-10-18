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
 * @brief SegyBulkReader.h contains the declaration of the SegyBulkReader class used for reading the trace sample data in the SEG-Y file.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReference.h"
#include "../Utilities/auth/TokenSource.h"

#include <string>
#include <map>

namespace segysdk::fileio
{

/**
 * @brief text file reader.
 *
 * Reads text file from local or cloud sdms based file
 *
 */
class SEGYSDK_DLL_PUBLIC GenericFileReader
{

public:
    /**
   * @brief Check if selected file exists.
   * 
   * @param fileName    Fully qualified name of input text file.    
   * @returns           bool true if file name indicates cloud storage (starts with sd:// ), otherwise false.
   **/
    static bool exists(const std::string& fileName);

    /**
   * @brief Check if selected file exists.
   * 
   * @param fileName    Fully qualified name of input text file.    
   * @param sdms_url    url to cloud hosted SDMS service. 
   * @param api_key     API KEY for cloud SDMS service connection. 
   * @param tokenSource Personal user token for accessing cloud SDMS service being fetched via TokenSource interface.
   * @returns           bool true if file name indicates cloud storage (starts with sd:// ), otherwise false.
   **/
    static bool exists(const std::string& fileName,
                       const std::string& sdms_url,
                       const std::string& api_key,
                       std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                       std::map<std::string, std::string> headers = std::map<std::string, std::string>());

    /**
   * @brief Check if selected file is local or cloud hosted.
   * 
   * @param fileName    Fully qualified name of input text file.    
   * @returns           bool true if file name indicates cloud storage (starts with sd:// ), otherwise false.
   **/
    static bool isSDDataset(const std::string& fileName);

public:
    /**
   * @brief Virtual destructor.
   */
    virtual ~GenericFileReader();

    /**
   * @brief get file reference for external file. This method does not use any caching and is intended only for file lookups.
   *   
   * @returns           fileio::FileReference The file reference for the current generic file.
   * @throws            std::invalid_argument if provided file name does not exists.
   **/
    virtual FileReference getFileReference() = 0;

    /**
   * @brief Read from external text file.
   *   
   * @returns           string with contents of the text file specified.
   * @throws            std::invalid_argument if provided file name does not exists.
   **/
    virtual std::string readText() = 0;

    /**
   * @brief Check if selected file exists.
   *  
   * @returns           bool true if file exists, false if it doesn't.
   **/
    virtual bool exists() = 0;

    /**
   * @brief Check if selected file exists.
   *  
   * @returns           size_t size of input file.
   * @throws            std::invalid_argument if input file does not exist.
   **/
    virtual size_t getSize() = 0;
};

} // namespace segysdk::fileio
