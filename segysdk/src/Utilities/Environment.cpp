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

#include "../Utilities/Environment.h"

#include "../segysdk_details/SegyException.h"
#include "../Logger/Logger.h"

#include <array>
#include <functional>
#include <iostream>
#include <memory>

namespace segysdk::utilities
{

std::shared_ptr<Environment> Environment::instance_;

Environment::~Environment()
{
    /* EMPTY */
}

Environment&
Environment::instance()
{
    if (!instance_) {
        instance_ = std::make_shared<Environment>();
    }

    return *instance_;
}

void
Environment::dump() const
{
    std::array<std::reference_wrapper<std::string const>, 34U> publicEnvVars = {
        EnvironmentVariableStringConsts::SEGYTOZGY_GENERATE_INDEX,
        EnvironmentVariableStringConsts::SEGYTOZGY_INSECURE_PRINT_TOKEN,
        EnvironmentVariableStringConsts::SEGYTOZGY_VERBOSITY,

        EnvironmentVariableStringConsts::SAUTH_SERVICE_TOKEN_V2_URL,
        EnvironmentVariableStringConsts::TOKEN_EXPIRATION_THRESHOLD_SECONDS,

        EnvironmentVariableStringConsts::SD_SVC_URL,
        
        EnvironmentVariableStringConsts::SD_SVC_SCOPE,
        EnvironmentVariableStringConsts::SD_READ_CACHE_PAGE_SIZE,
        EnvironmentVariableStringConsts::SD_READ_CACHE_MAX_PAGES,
        EnvironmentVariableStringConsts::SD_READ_CACHE_BLOCKS_PER_PAGE,

        EnvironmentVariableStringConsts::STORAGE_SVC_URL,
        EnvironmentVariableStringConsts::STORAGE_SVC_SCOPE,
        EnvironmentVariableStringConsts::OSDU_DATAPARTITIONID,

        EnvironmentVariableStringConsts::SEGYTOZGY_OUTPUT_PATH,

        EnvironmentVariableStringConsts::SEGYTOZGY_ZFP_COMPRESS,
        EnvironmentVariableStringConsts::SEGYTOZGY_ZFP_SNR,
        EnvironmentVariableStringConsts::SEGYTOZGY_ZFP_LOD_COMPRESS,
        EnvironmentVariableStringConsts::SEGYTOZGY_ZFP_LOD_SNR,

        EnvironmentVariableStringConsts::SEGYTOZGY_ZGY_Output_Parameters,

        EnvironmentVariableStringConsts::AUTHENTICATION_PLUGIN_FILE,
        
        EnvironmentVariableStringConsts::COMPUTE_POLYGON_OUTLINE,
        EnvironmentVariableStringConsts::OVERWRITE_POLYGON_OUTLINE,

        EnvironmentVariableStringConsts::CALCULATE_STATISTICS,
        EnvironmentVariableStringConsts::OVERWRITE_STATISTICS,

        EnvironmentVariableStringConsts::SEGYTOZGY_READER_COUNT,
        EnvironmentVariableStringConsts::SEGYTOZGY_READ_JOB_QUEUE_SIZE,
        EnvironmentVariableStringConsts::SEGYTOZGY_WRITER_COUNT,
        EnvironmentVariableStringConsts::SEGYTOZGY_BRICK_COLUMN_QUEUE_SIZE,
        EnvironmentVariableStringConsts::SEGYINDEX_SCANNER_LOOK_AHEAD,
        EnvironmentVariableStringConsts::SEGYINDEX_SCANNER_LOOK_AHEAD_THREADS,

        EnvironmentVariableStringConsts::EXTRACT_2D_NAVIGATION,
        EnvironmentVariableStringConsts::CALCULATE_STATISTICS,
        EnvironmentVariableStringConsts::OVERWRITE_STATISTICS,
        EnvironmentVariableStringConsts::USE_BIN_GRID_FOR_COMPUTE_POLYGON_OUTLINE
        };

    std::array<std::reference_wrapper<std::string const>, 2U> secretEnvVars = {
        EnvironmentVariableStringConsts::SD_SVC_TOKEN,
        EnvironmentVariableStringConsts::SD_SVC_API_KEY
    };

    std::stringstream ss{"settings; "};
    SEGYLOG_DEBUG("Environment variables:");
    for (const auto& var : publicEnvVars) {
        SEGYLOG_DEBUG(std::string(var) + "=[" + getRawValue(var) + "]");
        ss << std::string(var) << ": " << getRawValue(var) << "; ";
    }
    SEGYLOG_GSM_INFO(ss.str());

    if (insecurePrintTokenToDebug()) {
        for (const auto& var : secretEnvVars) {
            SEGYLOG_DEBUG(std::string(var) + "=[" + getRawValue(var) + "]");
        }
    }
    else {
        SEGYLOG_DEBUG(std::string("set ") + EnvironmentVariableStringConsts::SEGYTOZGY_INSECURE_PRINT_TOKEN + "=1 to print secret values");
    }
    SEGYLOG_DEBUG("END Environment variables");
}

bool
Environment::shouldGenerateIndex() const
{
    return !getRawValue(EnvironmentVariableStringConsts::SEGYTOZGY_GENERATE_INDEX).empty();
}

bool
Environment::insecurePrintTokenToDebug() const
{
    return !getRawValue(EnvironmentVariableStringConsts::SEGYTOZGY_INSECURE_PRINT_TOKEN).empty() && 
        getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_INSECURE_PRINT_TOKEN, 0) > 0;
}

int
Environment::segyToZgyVerbosity() const
{
    return getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_VERBOSITY, 0);
}

int
Environment::indexingVerbosity() const
{
    return getIntEnv(EnvironmentVariableStringConsts::INDEXING_VERBOSITY, 0);
}

std::string
Environment::sdToken() const
{

    return Environment::skipBearerPrefix(getRawValue(EnvironmentVariableStringConsts::SD_SVC_TOKEN));
}

std::string
Environment::sdTokenFile() const
{
    return getRawValue(EnvironmentVariableStringConsts::SD_SVC_TOKEN_FILE);
}

std::string
Environment::sdUrl() const
{
    return getRawValue(EnvironmentVariableStringConsts::SD_SVC_URL);
}

std::string
Environment::sdApiKey() const
{
    return getRawValue(EnvironmentVariableStringConsts::SD_SVC_API_KEY);
}

std::string
Environment::sdScope() const
{
    return getRawValue(EnvironmentVariableStringConsts::SD_SVC_SCOPE);
}

std::size_t
Environment::sdReadCachePageSize() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SD_READ_CACHE_PAGE_SIZE, 0));
}

std::size_t
Environment::sdReadCacheMaxPages() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SD_READ_CACHE_MAX_PAGES, 0));
}

std::size_t
Environment::sdReadCacheBlocksPerPage() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SD_READ_CACHE_BLOCKS_PER_PAGE, 1));
}


std::size_t
Environment::openZgyWriteThreads() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_IOCONTXT_WRITE_THREADS, 16));
}

std::size_t
Environment::openZgyBufferSize() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_IOCONTXT_BUFFER_SIZE, 32));
}

std::size_t
Environment::openZgyCPUThreads() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_IOCONTXT_CPU_THREADS, 16));
}

std::size_t
Environment::openZgyIOThreads() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_IOCONTXT_IO_THREADS, 16));
}

std::string
Environment::BrickSize() const
{
    return getStringEnv(EnvironmentVariableStringConsts::SEGYTOZGY_BRICKSIZE);
}

size_t
Environment::SegyIndexReadBufferSize() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYINDEX_READBUFFERSIZE, 0));
}

std::size_t
Environment::readerCount() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_READER_COUNT, 64));
}

std::size_t
Environment::readJobQueueSize() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_READ_JOB_QUEUE_SIZE, 2048));
}

std::size_t
Environment::writerCount() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_WRITER_COUNT, 32));
}

std::size_t
Environment::brickColumnQueueSize() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_BRICK_COLUMN_QUEUE_SIZE, 128));
}

std::size_t
Environment::scannnerLookAhead() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYINDEX_SCANNER_LOOK_AHEAD, 128));
}

std::size_t
Environment::scannnerLookAheadThreads() const
{
    return static_cast<std::size_t>(getIntEnv(EnvironmentVariableStringConsts::SEGYINDEX_SCANNER_LOOK_AHEAD_THREADS, 32));
}


std::string
Environment::tokenSvcUrl() const
{
    return getRawValue(EnvironmentVariableStringConsts::SAUTH_SERVICE_TOKEN_V2_URL);
}

int
Environment::tokenExpirationThresholdSeconds() const
{
    return getIntEnv(EnvironmentVariableStringConsts::TOKEN_EXPIRATION_THRESHOLD_SECONDS, 120);
}

std::string
Environment::storageSvcUrl() const
{
    return getRawValue(EnvironmentVariableStringConsts::STORAGE_SVC_URL);
}

std::string
Environment::osduDataPartitionId() const
{
    return getRawValue(EnvironmentVariableStringConsts::OSDU_DATAPARTITIONID);
}

std::string
Environment::subscriptionKey() const
{
    return getRawValue(EnvironmentVariableStringConsts::SUBSCRIPTION_KEY);
}

std::string
Environment::correlationId() const
{
    return getRawValue(EnvironmentVariableStringConsts::CORRELATION_ID);
}

std::string
Environment::outputPathOverrideOrDefault(const std::string& defaultValue) const
{
    const auto overrideValue = outputPathRaw1();
    return overrideValue.empty() ? defaultValue : overrideValue;
}

std::string
Environment::zgyOutputParameters() const
{
    return getStringEnv(EnvironmentVariableStringConsts::SEGYTOZGY_ZGY_Output_Parameters);
}

bool
Environment::zfpCompression() const
{
    return getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_ZFP_COMPRESS, 0) > 0;
}

int
Environment::zfpCompressionSnr() const
{
    return getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_ZFP_SNR, 0);
}

bool
Environment::zfpLodCompression() const
{
    return getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_ZFP_LOD_COMPRESS, 0) > 0;
}

int
Environment::zfpLodCompressionSnr() const
{
    return getIntEnv(EnvironmentVariableStringConsts::SEGYTOZGY_ZFP_LOD_SNR, 0);
}


std::string
Environment::SegyLocalTestDataPath() const
{
    return getRawValue(EnvironmentVariableStringConsts::SEGY_LOCALTESTDATA_PATH);
}

std::string
Environment::ShowTimingInfo() const
{
    return getRawValue(EnvironmentVariableStringConsts::ShowTimingInfo);
}

std::string
Environment::segysdkConfigurationFile() const
{
    return getStringEnv(EnvironmentVariableStringConsts::SEGYSDK_CONFIGURATION_FILE);
}

bool
Environment::computePolygonOutline() const
{
    return getIntEnv(EnvironmentVariableStringConsts::COMPUTE_POLYGON_OUTLINE, 0) > 0;
}

bool
Environment::overwritePolygonOutline() const
{
    return getIntEnv(EnvironmentVariableStringConsts::OVERWRITE_POLYGON_OUTLINE, 0) > 0;
}

std::string
Environment::extract2DNavigationPluginPath() const
{
    return getStringEnv(EnvironmentVariableStringConsts::EXTRACT_2D_NAVIGATION_PLUGIN_PATH);
}

bool
Environment::extract2DNavigation() const
{
    return getIntEnv(EnvironmentVariableStringConsts::EXTRACT_2D_NAVIGATION, 0) > 0;
}

bool
Environment::overwriteIndex() const
{
    return getIntEnv(EnvironmentVariableStringConsts::OVERWRITE_INDEX, 0) > 0;
}

bool
Environment::calculateStatistics() const
{
    return getIntEnv(EnvironmentVariableStringConsts::CALCULATE_STATISTICS, 0) > 0;
}

bool
Environment::overwriteStatistics() const
{
    return getIntEnv(EnvironmentVariableStringConsts::OVERWRITE_STATISTICS, 0) > 0;
}

bool
Environment::slbPluginsVerbosity() const
{
    return getIntEnv(EnvironmentVariableStringConsts::SLB_PLUGIN_VERBOSITY, 0) > 0;
}

bool
Environment::useBinGridForComputePolygonOutline() const
{
    return getIntEnv(EnvironmentVariableStringConsts::USE_BIN_GRID_FOR_COMPUTE_POLYGON_OUTLINE, 0) > 0;
}

double
Environment::progressPrecision() const
{
    return getDoubleEnv(EnvironmentVariableStringConsts::PROGRESS_PRECISION, 1);
}

std::string
Environment::outputPathRaw1() const
{
    return getRawValue(EnvironmentVariableStringConsts::SEGYTOZGY_OUTPUT_PATH);
}

std::string
Environment::authenticationPluginFile() const
{
    return getRawValue(EnvironmentVariableStringConsts::AUTHENTICATION_PLUGIN_FILE);
}

std::string
Environment::authenticationSafetyThreshold() const
{
    return getStringEnv(EnvironmentVariableStringConsts::TOKEN_EXPIRATION_THRESHOLD_SECONDS);
}

std::string
Environment::getRawValue(const std::string& variableName) const
{
    return getStringEnv(variableName.c_str());
}

std::string
Environment::skipBearerPrefix(const std::string& auth)
{
    const std::size_t pos = auth.find("Bearer ");
    if (pos != std::string::npos)
        return auth.substr(pos + 7);

    return auth;
}

std::string
Environment::userId() const
{
    return getStringEnv(EnvironmentVariableStringConsts::USER_ID);
}

} // namespace segysdk::utilities
