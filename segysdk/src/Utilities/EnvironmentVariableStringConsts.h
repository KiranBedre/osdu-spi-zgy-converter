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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC EnvironmentVariableStringConsts
{
public:
    inline static const std::string SD_SVC_URL = "SD_SVC_URL";
    inline static const std::string SD_SVC_API_KEY = "SD_SVC_API_KEY";
    inline static const std::string SD_SVC_TOKEN = "SD_SVC_TOKEN";
    inline static const std::string STORAGE_SVC_URL = "STORAGE_SVC_URL";
    inline static const std::string STORAGE_SVC_SCOPE = "STORAGE_SVC_SCOPE";
    inline static const std::string SD_SVC_SCOPE = "SD_SVC_SCOPE";
    inline static const std::string SD_READ_CACHE_PAGE_SIZE = "SD_READ_CACHE_PAGE_SIZE";
    inline static const std::string SD_READ_CACHE_MAX_PAGES = "SD_READ_CACHE_MAX_PAGES";
    inline static const std::string SD_READ_CACHE_BLOCKS_PER_PAGE = "SD_READ_CACHE_BLOCKS_PER_PAGE";
    inline static const std::string SD_SVC_TOKEN_FILE = "SD_SVC_TOKEN_FILE";

    inline static const std::string SAUTH_SERVICE_TOKEN_V2_URL = "SAUTH_SERVICE_TOKEN_V2_URL";
    inline static const std::string TOKEN_EXPIRATION_THRESHOLD_SECONDS = "TOKEN_EXPIRATION_THRESHOLD_SECONDS";
    inline static const std::string OSDU_DATAPARTITIONID = "OSDU_DATAPARTITIONID";
    inline static const std::string CORRELATION_ID = "CORRELATION_ID";

    inline static const std::string SUBSCRIPTION_KEY = "SUBSCRIPTION_KEY";

    inline static const std::string INDEXING_VERBOSITY = "INDEXING_VERBOSITY";
    inline static const std::string INDEXING_INSECURE_PRINT_TOKEN = "INDEXING_INSECURE_PRINT_TOKEN";
    inline static const std::string SEGYINDEX_READBUFFERSIZE = "SEGYINDEX_READBUFFERSIZE";
    inline static const std::string SEGYTOZGY_VERBOSITY = "SEGYTOZGY_VERBOSITY";
    inline static const std::string SEGYTOZGY_INSECURE_PRINT_TOKEN = "SEGYTOZGY_INSECURE_PRINT_TOKEN";
    inline static const std::string SEGYTOZGY_GENERATE_INDEX = "SEGYTOZGY_GENERATE_INDEX";
    inline static const std::string SEGYTOZGY_ZGY_Output_Parameters = "SEGYTOZGY_ZGY_Output_Parameters";
    inline static const std::string SEGYTOZGY_ZFP_COMPRESS = "SEGYTOZGY_ZFP_COMPRESS";
    inline static const std::string SEGYTOZGY_ZFP_SNR = "SEGYTOZGY_ZFP_SNR";
    inline static const std::string SEGYTOZGY_ZFP_LOD_COMPRESS = "SEGYTOZGY_ZFP_LOD_COMPRESS";
    inline static const std::string SEGYTOZGY_ZFP_LOD_SNR = "SEGYTOZGY_ZFP_LOD_SNR";
    inline static const std::string SEGYTOZGY_OUTPUT_PATH = "SEGYTOZGY_OUTPUT_PATH";

    inline static const std::string SEGYTOZGY_BRICKSIZE = "SEGYTOZGY_BRICKSIZE";

    inline static const std::string SEGY_LOCALTESTDATA_PATH = "SEGY_LOCALTESTDATA_PATH";

    inline static const std::string ShowTimingInfo = "ShowTimingInfo";

    inline static const std::string SEGYSDK_CONFIGURATION_FILE = "SEGYSDK_CONFIGURATION_FILE";

    inline static const std::string AUTHENTICATION_PLUGIN_FILE = "AUTHENTICATION_PLUGIN_FILE";

    inline static const std::string COMPUTE_POLYGON_OUTLINE = "COMPUTE_POLYGON_OUTLINE";
    inline static const std::string OVERWRITE_POLYGON_OUTLINE = "OVERWRITE_POLYGON_OUTLINE";
    inline static const std::string OVERWRITE_INDEX = "OVERWRITE_INDEX";
    inline static const std::string USE_BIN_GRID_FOR_COMPUTE_POLYGON_OUTLINE = "USE_BIN_GRID_FOR_COMPUTE_POLYGON_OUTLINE";

    inline static const std::string EXTRACT_2D_NAVIGATION_PLUGIN_PATH = "EXTRACT_2D_NAVIGATION_PLUGIN_PATH";
    inline static const std::string EXTRACT_2D_NAVIGATION = "EXTRACT_2D_NAVIGATION";

    inline static const std::string PROGRESS_PRECISION = "PROGRESS_PRECISION";

    inline static const std::string CALCULATE_STATISTICS = "CALCULATE_STATISTICS";
    inline static const std::string OVERWRITE_STATISTICS = "OVERWRITE_STATISTICS";

    inline static const std::string SLB_PLUGIN_VERBOSITY = "SLB_PLUGIN_VERBOSITY";

    // Variables to control OpenZGY iocontext/Memory requirements
    inline static const std::string SEGYTOZGY_IOCONTXT_BUFFER_SIZE = "SEGYTOZGY_IOCONTXT_BUFFER_SIZE";
    inline static const std::string SEGYTOZGY_IOCONTXT_WRITE_THREADS = "SEGYTOZGY_IOCONTXT_WRITE_THREADS";
    inline static const std::string SEGYTOZGY_IOCONTXT_CPU_THREADS = "SEGYTOZGY_IOCONTXT_CPU_THREADS";
    inline static const std::string SEGYTOZGY_IOCONTXT_IO_THREADS = "SEGYTOZGY_IOCONTXT_IO_THREADS";

    // Variables to control OpenZGY iocontext/Memory requirements
    inline static const std::string SEGYTOZGY_READER_COUNT = "SEGYTOZGY_READER_COUNT";
    inline static const std::string SEGYTOZGY_READ_JOB_QUEUE_SIZE = "SEGYTOZGY_READ_JOB_QUEUE_SIZE";
    inline static const std::string SEGYTOZGY_WRITER_COUNT = "SEGYTOZGY_WRITER_COUNT";
    inline static const std::string SEGYTOZGY_BRICK_COLUMN_QUEUE_SIZE = "SEGYTOZGY_BRICK_COLUMN_QUEUE_SIZE";
    inline static const std::string SEGYINDEX_SCANNER_LOOK_AHEAD = "SEGYINDEX_SCANNER_LOOK_AHEAD";
    inline static const std::string SEGYINDEX_SCANNER_LOOK_AHEAD_THREADS = "SEGYINDEX_SCANNER_LOOK_AHEAD_THREADS";
    

    inline static const std::string USER_ID = "USER_ID";
};

} // namespace segysdk::utilities
