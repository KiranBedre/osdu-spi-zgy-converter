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

#include "../tools/SampleCode/Common/SessionUtilities.h"

#include "../Utilities/InputParser.h"
#include "../Utilities/EnvironmentVariableStringConsts.h"

#include <cmath>
#include <iostream>

std::vector<std::string>
getTextualHeadersFromDisk(const std::unique_ptr<segysdk::api::SegySession>& session)
{
    auto headers = session->getAsciiHeadersByLine();
    return headers;
}

void
ShowTextualHeaders(const std::vector<std::string>& textualHeaders)
{
    if (!textualHeaders.empty()) {
        std::cout << "=====================================================================" << std::endl;
        std::cout << "=====                      Textual Headers                      =====" << std::endl;
        std::cout << "=====================================================================" << std::endl;

        for (const auto& hdrLine : textualHeaders) {
            std::cout << hdrLine << std::endl;
        }
    }
}

std::vector<segysdk::api::HeaderDataStruct>
getBinaryHeadersFromDisk(const std::unique_ptr<segysdk::api::SegySession>& session)
{
    std::vector<segysdk::api::HeaderDataStruct> headers = session->getBinaryHeaders();

    return headers;
}

// just output headers to std::cout
void
ShowBinaryHeaders(const std::vector<segysdk::api::HeaderDataStruct>& binaryHeaders)
{
    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====                      Binary Header                        =====" << std::endl;
    std::cout << "=====================================================================" << std::endl;
    const std::string dots = " ..........................................................";
    for (const auto& hdr : binaryHeaders) {
        constexpr int width = 35;
        std::string displayHdrId = hdr.id;
        const auto offset = hdr.offset;
        const auto end = hdr.end;
        std::cout << "[" << offset << " - " << end << "]   ";
        const auto strLen = displayHdrId.size();
        std::string myDots = dots.substr(0, width - strLen);
        std::cout << " " + displayHdrId << myDots << " : ";
        const auto value = static_cast<int>(hdr.value);
        std::cout << value << std::endl;
    }
}

std::vector<std::vector<segysdk::api::HeaderDataStruct>>
getRawTraceHeadersFromDisk(const std::unique_ptr<segysdk::api::SegySession>& session)
{
    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====                  Raw Trace Headers                        =====" << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;
    // start position for reading
    long startTrace = 0;
    // last position for reading
    int traceCount = 1000;
    std::cout << "Start scan from trace#   [0] : ";
    std::string answer;
    std::cin >> answer;
    if (!answer.empty()) {
        startTrace = std::stoi(answer);
        if (startTrace == -1) {
            std::cout << "answer empty" << std::endl;
            return {};
        }
    }
    std::cout << std::endl
              << "Number of traces to scan? [1000]: ";
    std::cin >> answer;
    if (!answer.empty()) {
        traceCount = std::stoi(answer);
        if (traceCount == -1) {
            std::cout << "number of traces invalid" << std::endl;
            return {};
        }
    }

    // find raw trace headers
    auto result = session->getRawTraceHeaders(startTrace, traceCount);
    return result;
}

// just output headers to std::cout
void
ShowRawTraceHeaders(const std::unique_ptr<segysdk::api::SegySession>& session)
{
    const auto rawTraceHeaders = getRawTraceHeadersFromDisk(session);
    size_t traceNo = rawTraceHeaders[0][0].offset;
    typedef std::numeric_limits<double> dbl;
    for (const auto& traceData : rawTraceHeaders) { // all requested traces
        std::cout << "TraceNo: " << traceNo++ << std::endl;
        for (const auto& td : traceData) { // single trace fields
            std::cout.precision(dbl::max_digits10);
            std::cout << "   Id:" << td.id << "  Offset: " << td.offset << " End:" << td.end << "   Value: " << td.value << std::endl;
        }
    }
}

int
getIntFromString(const std::string& text)
{
    try {
        std::string::size_type sz; // alias of size_t
        const long value = std::stoi(text, &sz);

        return value;
    }
    catch (...) {
        std::cout << "Illegal integer value entered!" << std::endl;
        return 0;
    }
}

long
getLongFromString(const std::string& text)
{
    try {
        std::string::size_type sz; // alias of size_t
        const long value = std::stol(text, &sz);

        return value;
    }
    catch (...) {
        std::cout << "Illegal integer value entered!" << std::endl;
        return -1;
    }
}

/**
 * \brief Method to display specific trace headers as they will be interpreted by the segy lilbrary.
 *
 * \detail This affects any scaled or overridden trace header values where the returned values might be different to
 * the raw values actually stored in the trace header.
 *
 * \param session unique pointer to a SegySession instance
 */
void
ShowInterpretedTraceHeaders(const std::unique_ptr<segysdk::api::SegySession>& session)
{

    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====           Interpreted Trace Headers                       =====" << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;

    long startTrace = 0;
    int traceCount = 1000;
    std::cout << "Start scan from trace#   [0]               : ";
    std::string answer;
    std::cin >> answer;
    if (!answer.empty()) {
        startTrace = getLongFromString(answer);
        if (startTrace == -1) {
            return;
        }
    }
    std::cout << std::endl
              << "Number of traces to scan? [1000]: ";
    std::cin >> answer;
    if (!answer.empty()) {
        traceCount = getIntFromString(answer);
        if (traceCount == -1) {
            return;
        }
    }

    try {
        const auto result = session->getScaledTraceHeaders(startTrace, traceCount);

        typedef std::numeric_limits<double> dbl;

        long traceNo = startTrace;
        for (const auto& traceData : result) { // all requested traces
            std::cout << "TraceNo: " << traceNo++ << std::endl;
            for (const auto& td : traceData) { // single trace fields
                std::cout.precision(dbl::digits10);
                const double roundedValue = round(td.value * 1000.0) / 1000.0; // Round to show only 3 digits after decimal point

                std::cout << "   Id:" << td.id << "  Offset: " << td.offset << " End:" << td.end << "   Value: " << roundedValue << std::endl;
            }
        }
    }
    catch (const std::logic_error& le) {
        std::cerr << "Logical error: " << le.what() << std::endl;
    }
}

void
DisplayMenu()
{
    std::cout << std::endl
              << std::endl;
    std::cout << "---------------------------------------------------------------------" << std::endl;
    std::cout << "1. Display textual headers" << std::endl;
    std::cout << "2. Display binary headers" << std::endl;
    std::cout << "3. Display raw trace header values" << std::endl;
    std::cout << "4. Display interpreted trace header values" << std::endl;
    std::cout << "Q. Quit tests" << std::endl;
    std::cout << std::endl;
    std::cout << "---------------------------------------------------------------------" << std::endl
              << std::endl;
    std::cout << "Please enter function choice: ";
}

void
print_usage(const char* exec_name, int argc)
{
    std::cout << exec_name << " called with " << std::to_string(argc - 1) << " parameters." << std::endl;

    std::cout << "Usage: " << std::endl;
    std::cout << "\tFor OSDU Data Platform: " << std::endl;
    std::cout << "\t\t" << exec_name << " -osdu <FileCollection.SEGY.json>" << std::endl;

    std::cout << "To access data in cloud SDMS the following environment variables must be defined:" << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_URL << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_API_KEY << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN << std::endl;
}

int
main(int argc, char* argv[])
{
    const segysdk::utilities::InputParser input(argc, argv);

    std::vector<std::string> segyFileList;
    std::string segyFile;

    std::string jsonFile;

    // Check if user has provided valid runtime parameters. If not, provide usage information
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("-help") || argc < 2) {
        print_usage(argv[0], argc);
        return -1;
    }

    std::string osduFileCollection;
    const std::string& osduFileName = input.getCmdOption("-osdu");
    if (!osduFileName.empty()) {

        osduFileCollection = osduFileName;
    }
    else {
        std::cout << "Input FileCollection.SEGY parameter must be a valid file name!" << std::endl;
        return -1;
    }

    const std::unique_ptr<segysdk::api::SegySession> session = SampleCode::Common::SessionUtilities::GetSegySessionFromFileCollectionJson(osduFileCollection);

    if (session == nullptr) {
        std::cout << "MetadataBrowser: Unable to create a valid SEG-Y session, exiting!" << std::endl;
        return -10;
    }

    try {
        bool keepLooping = true;
        do {
            DisplayMenu();
            char option;
            std::cin >> option;
            switch (option) {
            case '1':
                // get textual headers from segy file
                ShowTextualHeaders(getTextualHeadersFromDisk(session));
                break;
            case '2':
                // get textual headers from segy file
                ShowBinaryHeaders(getBinaryHeadersFromDisk(session));
                break;
            case '3':
                // get raw trace headers from segy file
                ShowRawTraceHeaders(session);
                break;
            case '4':
                // get raw trace headers from segy file
                ShowInterpretedTraceHeaders(session);
                break;
            case 'q':
                std::cout << "Quit" << std::endl;
                keepLooping = false;
                break;

            default:
                std::cout << "Invalid menu option ... " << std::endl;
                break;
            }

        } while (keepLooping);
    }
    catch (std::exception& ex) {
        std::cout << std::endl
                  << "Error extracting metadata: " << ex.what() << std::endl;
        return -100;
    }

    return 0;
}
