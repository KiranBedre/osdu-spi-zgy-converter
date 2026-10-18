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

#include "../Utilities/EnvironmentVariableStringConsts.h"
#include "../Utilities/EnvironmentVariables.h"

#include <memory>
#include <string>

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC Environment : EnvironmentVariable, EnvironmentVariableStringConsts
{
public:
    virtual ~Environment();

    static Environment& instance();

    void dump() const;

    bool shouldGenerateIndex() const;
    bool insecurePrintTokenToDebug() const;
    int segyToZgyVerbosity() const;
    int indexingVerbosity() const;

    std::string sdToken() const;
    std::string sdTokenFile() const;

    std::string sdUrl() const;
    std::string sdApiKey() const;
    std::string sdScope() const;
    size_t sdReadCachePageSize() const;
    size_t sdReadCacheMaxPages() const;
    size_t sdReadCacheBlocksPerPage() const;
    size_t openZgyWriteThreads() const;
    size_t openZgyBufferSize() const;
    size_t openZgyCPUThreads() const;
    size_t openZgyIOThreads() const;

    std::string BrickSize() const;

    std::size_t readerCount() const;
    std::size_t readJobQueueSize() const;
    std::size_t writerCount() const;
    std::size_t brickColumnQueueSize() const;
    std::size_t scannnerLookAhead() const;
    std::size_t scannnerLookAheadThreads() const;

    size_t SegyIndexReadBufferSize() const;

    std::string tokenSvcUrl() const;
    int tokenExpirationThresholdSeconds() const;

    std::string storageSvcUrl() const;
    std::string osduDataPartitionId() const;
    std::string correlationId() const;
    std::string subscriptionKey() const;

    std::string outputPathOverrideOrDefault(const std::string& defaultValue) const;

    std::string zgyOutputParameters() const;
    bool zfpCompression() const;
    int zfpCompressionSnr() const;
    bool zfpLodCompression() const;
    int zfpLodCompressionSnr() const;

    std::string SegyLocalTestDataPath() const;

    std::string ShowTimingInfo() const;

    std::string segysdkConfigurationFile() const;

    std::string outputPathRaw1() const;

    std::string authenticationPluginFile() const;

    std::string authenticationSafetyThreshold() const;

    bool computePolygonOutline() const;
    bool overwritePolygonOutline() const;

    std::string extract2DNavigationPluginPath() const;
    bool extract2DNavigation() const;

    bool overwriteIndex() const;

    bool calculateStatistics() const;
    bool overwriteStatistics() const;

    bool slbPluginsVerbosity() const;

    std::string userId() const;
    bool useBinGridForComputePolygonOutline() const;

    double progressPrecision() const;

private:
    static std::shared_ptr<Environment> instance_;

    virtual std::string getRawValue(const std::string& variableName) const;

    static std::string skipBearerPrefix(const std::string& auth);
};

} // namespace segysdk::utilities
