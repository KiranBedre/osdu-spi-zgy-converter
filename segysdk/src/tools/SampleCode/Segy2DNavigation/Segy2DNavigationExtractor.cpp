/*
Copyright 2024 Schlumberger

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

#include "../SegyIO/SegyIOException.h"

#include "../Utilities/Environment.h"
#include "../Utilities/EnvironmentVariableStringConsts.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/InputParser.h"
#include "../Utilities/SessionUtilities.h"
#include "../FileIO/GenericFileManager.h"
#include "../Logger/Logger.h"

#include <iostream>

void
print_usage(const char* exec_name, int argc)
{
    std::cout << exec_name << " called with " << std::to_string(argc - 1) << " parameters." << std::endl;

    std::cout << "Usage: " << std::endl;
    std::cout << "\tFor OSDU Data Platform: " << std::endl;
    std::cout << "\t\t" << exec_name << " -osdu <FileCollectionId> -seismic_trace_data <SeismicTraceDataId>  -seismic_trace_data <SeismicTraceDataId> -seismic_2d_interpretation_set_idinterp_2d <Seismic2DInterpretationSetId> [-verbose=0|1] " << std::endl;

    std::cout << "To access data in cloud SDMS the following environment variables must be defined:" << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_URL << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_API_KEY << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN << std::endl;
}

bool
isValidOsduId(const std::string& pathOrId, const std::string& osduPartition = "")
{
    const auto result = segysdk::utilities::StringUtils::split(pathOrId, ":");
    if (result.size() < 3 and result.size() >= 5) {
        return false;
    }
    
    if (!osduPartition.empty()) {
        return result[0] == osduPartition;
    }

    return true;
}

void
beforeExitCleanUp()
{
    // Report and gracefully stop the Logger in case of exit call
    try {
        auto& log = segysdk::logger::Logger::getInstance();
        log.stopLogWriterDispatcher();
        segysdk::fileio::GenericFileManager::reset();
    }
    catch (const std::exception& ex) {
        std::cerr << "Logger exiting exception:" << ex.what() << std::endl;
    }
}

int
main(int argc, char** argv)
{
    // So all logger writers will be able to report messages and exit
    segysdk::logger::LoggerSettings settings{};
    segysdk::logger::initializeLogger(settings);

    const segysdk::utilities::InputParser input(argc, argv);

    std::vector<std::string> segyFileList;
    std::string segyFile;

    std::string jsonFile;
    int exitCode = 0;
    // Check if user has provided valid runtime parameters. If not, provide usage information
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("-help") || argc < 2) {
        print_usage(argv[0], argc);
        beforeExitCleanUp();
        return -1;
    }

    const std::string osduPartition = segysdk::utilities::Environment::instance().osduDataPartitionId();

    std::string osduFileCollection;
    if (input.cmdOptionExists("-osdu")) {
        osduFileCollection = input.getCmdOption("-osdu");
    }
    if (osduFileCollection.empty()) {
        std::string msg = "Input FileCollection.SEGY parameter must be a valid file name or Id!";
        SEGYLOG_ERROR(msg);
        SEGYLOG_GSM_ERROR(msg);
        exitCode = - 1;
    }

    std::string osduSeismicTraceData;
    if (input.hasValidOption("-seismic_trace_data", ":work-product-component--SeismicTraceData:")) {
        osduSeismicTraceData = input.getCmdOption("-seismic_trace_data");
    }
    if (osduSeismicTraceData.empty()) {
        std::string msg = "Input SeismicTraceData parameter must be a valid file name or Id";
        SEGYLOG_ERROR(msg);
        SEGYLOG_GSM_ERROR(msg);
        exitCode = -1;
    }

    std::string osduInterpretation2d;
    if (input.hasValidOption("-seismic_2d_interpretation_set_id", ":master-data--Seismic2DInterpretationSet:")) {
        osduInterpretation2d = input.getCmdOption("-seismic_2d_interpretation_set_id");
    }
    if (osduInterpretation2d.empty()) {
        std::string msg = "Input Interpretation2D parameter must be a valid file name or Id";
        SEGYLOG_ERROR(msg);
        SEGYLOG_GSM_ERROR(msg);
        exitCode = -1;
    }
    if (exitCode == 0) {
        try {
            const auto session = segysdk::utilities::SessionUtilities::GetSegySessionFromFileCollectionJson(osduFileCollection);

            if (session == nullptr) {
                std::cout << "SegyIndexer: Unable to create a valid SEG-Y session, exiting!\n";
                exitCode = -1;
            } else {
                const auto success = session->extract2DNavigation(osduFileCollection, osduSeismicTraceData, osduInterpretation2d);
                exitCode = success ? 0 : -1;
            }
        }
        catch (segysdk::SegyInvalidArgumentException& ex) {
            std::string msg = "Invalid argument passed to generateIndexFile! Error: " + std::string(ex.what());
            SEGYLOG_ERROR(msg);
            SEGYLOG_GSM_ERROR(msg);
            exitCode = -70;
        }
        catch (segysdk::segyio::SegyRuntimeException& ex) {
            std::string msg = "Segy Runtime exception! Error: " + std::string(ex.what());
            SEGYLOG_ERROR(msg);
            SEGYLOG_GSM_ERROR(msg);
            exitCode = -180;
        }
        catch (SegyException& ex) {
            std::string msg = "Exception when executing extract2DNavigation! Error: " + std::string(ex.what());
            SEGYLOG_ERROR(msg);
            SEGYLOG_GSM_ERROR(msg);
            exitCode = -90;
        }
        catch (std::exception& ex) {
            std::string msg = "Error encountered during extract2DNavigation! Error: " + std::string(ex.what());
            SEGYLOG_ERROR(msg);
            SEGYLOG_GSM_ERROR(msg);
            exitCode = -100;
        }
    }

    beforeExitCleanUp();
    return exitCode;
}
